#include "main.h"

bool is_symb(char c) {
  return !std::isalnum(c) && c != '<' && c != '>';
}

void reverseString(std::string &str) {
  int start = 0;
  int end = str.length() - 1;

  while (start < end) {
      // Swap characters at start and end
      std::swap(str[start], str[end]);
      start++;
      end--;
  }
}

std::string getTokRep(int n) {
  std::string result;

  while (n >= 94) {
    result.push_back((n % 94) + 33);
    n /= 94;
  }

  result.push_back(n + 33);
  reverseString(result);

  return result;
}

std::vector<char> rftv(char* filename) {

  std::ifstream file(filename, std::ios::binary | std::ios::ate); // open as binary and at end of file

  if (!file.is_open()) throw std::runtime_error("Could not open file");
  
  std::streamsize size = file.tellg();
  file.seekg(0, std::ios::beg);

  if (size <= 0) throw std::runtime_error("File empty or invalid size");

  std::vector<char> buffer(size);

  if (!file.read(buffer.data(), size)) throw std::runtime_error("Error while reading file");
  else std::cout << "Read " << size << " bytes" << std::endl;

  file.close();

  return buffer;
}

std::unordered_map<std::string, std::string> read_vocab(char* vocab_filename, bool tokenLookup) {
  
  std::unordered_map<std::string, std::string> vocab;
  vocab.reserve(50000);
  vocab.rehash(100000);
  
  std::ifstream file(vocab_filename);
  if (!file.is_open()) throw std::runtime_error("Could not open file");

  std::string word, token;
  while (file >> word >> token) {
    if (tokenLookup) vocab[token] = word;
    else vocab[word] = token;
  }

  file.close();

  return vocab;
}

int generate_vocabulary(char* filename) {
  
  // Read corpus file into vector
  std::vector<char> raw_text = rftv(filename);

  // Build dictionary of unique words and count frequency
  std::unordered_map<std::string, int> unique_words;
  std::string word;
  std::istringstream stream(std::string(raw_text.begin(), raw_text.end()));
  while (stream >> word) unique_words[word]++;

  // Split words into characters, mark ends with <>, 
  std::vector<std::vector<std::string>> split_strings;
  std::vector<int> word_counts;

  for (auto word = unique_words.begin(); word != unique_words.end(); word++) {
    std::vector<std::string> characters;

    for (int j = 0; j < word->first.length(); j++) {
      characters.push_back(std::string(1, word->first[j]));
    }

    characters.push_back("<>");
    split_strings.push_back(characters);
    word_counts.push_back(word->second);
  }

  // Initialize the vocabulary with unique characters
  std::vector<std::string> vocabulary;
  // Initialize with all printable ASCII characters (33-126) except whitespace
  for (int i = 33; i <= 126; i++) {
    if (!std::isspace(i)) {
      vocabulary.push_back(std::string(1, static_cast<char>(i)));
    }
  }

  // Add the special end token
  vocabulary.push_back("<>");

  int iter = 0;

  while (vocabulary.size() < 50000) {
    std::unordered_map<std::string, int> bigrams;

    // Find bigrams of characters in split_words and counts of bigrams
    // Track the most frequent bigram
    int max_val = 0;
    std::string max_key = "";

    for (int i = 0; i < split_strings.size(); i++) {
      for (int j = 0; j < split_strings[i].size() - 1; j++) {
        if (is_symb(split_strings[i][j][0]) || is_symb(split_strings[i][j+1][0])) continue;
        std::string ab = split_strings[i][j] + split_strings[i][j+1];
        int count = bigrams[ab] += word_counts[i];

        if (count > max_val) {
          max_val = count;
          max_key = ab;
        }
      }
    }

    if (bigrams.empty()) break;

    vocabulary.push_back(max_key);
    
    // Merge all instances of the most frequent bigram
    for (size_t i = 0; i < split_strings.size(); i++) {
      for (size_t j = 0; j < split_strings[i].size() - 1; j++) {
      std::string_view first = split_strings[i][j];
      std::string_view second = split_strings[i][j + 1];
      
      if ((first.length() + second.length() == max_key.length()) && (std::string(first) + std::string(second) == max_key)) {
        split_strings[i][j] = max_key; // Store the merged string
        split_strings[i].erase(split_strings[i].begin() + j + 1);
        }
      }
    }

    std::cout << "(" << iter++ << ") " << max_key << std::endl; 
  }

  // Write vocabulary to file for storage
  std::ofstream output_file("vocabulary.tokens");

  int n = 0;
  for (std::string s : vocabulary) {
    output_file << s << " " << getTokRep(n++) << std::endl;
  }

  output_file.close();

  return 0;
}

int encode(char* input_filename, char* vocabulary_filename, char* output_filename) {
  std::vector<char> input_data = rftv(input_filename);

  // Normalize smart quotes to straight quotes
  for (size_t i = 0; i < input_data.size(); i++) {
    // Check for UTF-8 sequences of smart quotes (they start with E2 80)
    if (i + 2 < input_data.size() && 
        static_cast<unsigned char>(input_data[i]) == 0xE2 && 
        static_cast<unsigned char>(input_data[i+1]) == 0x80) {
      
      unsigned char third_byte = static_cast<unsigned char>(input_data[i+2]);
      
      // Smart double quotes (left " = E2 80 9C, right " = E2 80 9D)
      if (third_byte == 0x9C || third_byte == 0x9D) {
        input_data[i] = '"';
        input_data.erase(input_data.begin() + i + 1, input_data.begin() + i + 3);
        i++; // Move past the replaced quote
        continue;
      }
      
      // Smart single quotes (left ' = E2 80 98, right ' = E2 80 99)
      if (third_byte == 0x98 || third_byte == 0x99) {
        input_data[i] = '\'';
        input_data.erase(input_data.begin() + i + 1, input_data.begin() + i + 3);
        i++; // Move past the replaced quote
        continue;
      }
    }
    
    // Handle any other non-ASCII characters
    if (static_cast<unsigned char>(input_data[i]) < 32 || 
        static_cast<unsigned char>(input_data[i]) > 126) {
      input_data[i] = '?';
    }
  }

  std::unordered_map<std::string, std::string> vocab = read_vocab(vocabulary_filename, false);

  RadixTrie trie;
  for (const auto& vocab_w : vocab) trie.insert(vocab_w.first);
  
  std::string input_str(input_data.begin(), input_data.end());
  std::vector<std::string> tokens;
  std::unordered_map<std::string, std::string> prefix_cache;

  // Process the input string word by word (whitespace-delimited)
  size_t pos = 0;
  while (pos < input_str.size()) {
    // Skip leading whitespace
    while (pos < input_str.size() && std::isspace(input_str[pos])) pos++;
    if (pos >= input_str.size()) break;
    
    // Extract word
    size_t end = pos;
    while (end < input_str.size() && !std::isspace(input_str[end])) end++;
    std::string current_word = input_str.substr(pos, end - pos) + "<>";
    
    pos = end;

    while (!current_word.empty()) {
      
      std::string max_subword;
      // Check cache first
      if (prefix_cache.find(current_word) != prefix_cache.end()) {
        max_subword = prefix_cache[current_word];
      } else {
        max_subword = trie.longest_prefix(current_word);
        prefix_cache[current_word] = max_subword;
      }

      if (!max_subword.empty()) {
        tokens.push_back(vocab[max_subword]);
        current_word = current_word.substr(max_subword.length()); // Chop off the subword from the current word
      } else {
        // If no subword is found, add the first character as a subword
        tokens.push_back(vocab[std::string(1, current_word[0])]);
        current_word = current_word.substr(1);
      }
    }
  }

  std::ofstream output_file(output_filename);
  for (const auto& token : tokens) {
    output_file << token << " ";
  }
  output_file.close();

  return 0;
}
           
int decode(char* input_filename, char* vocabulary_filename, char* output_filename) {
  // Read input file into a vector
  std::vector<char> input_data = rftv(input_filename);
  
  // Read vocabulary into a vector of strings
  std::unordered_map<std::string, std::string> vocab = read_vocab(vocabulary_filename, true);

  // Convert input data to a string

  std::vector<char> result;
  result.reserve(input_data.size());  // Reserve enough space for the result string

  std::string word;
  std::istringstream stream(std::string(input_data.begin(), input_data.end()));
  
  while (stream >> word) {
      // Get the word's index from the vocab_map and append the corresponding word
    for (char c : vocab[word]) result.push_back(c);
  }

  // Replace all occurrences of "<>" with a space
  std::vector<char> final_result;
  final_result.reserve(result.size());  // Reserve space for the final result

  for (size_t i = 0; i < result.size(); ++i) {
      // If we encounter "<>", replace it with a space
      if (i + 1 < result.size() && result[i] == '<' && result[i + 1] == '>') {
          final_result.push_back(' ');  // Replace with space
          i++;  // Skip the next character '>'
      } else {
          final_result.push_back(result[i]);  // Copy current character as is
      }
  }

  // Convert final_result back to a string and write to output file
  std::ofstream output_file(output_filename);
  output_file.write(final_result.data(), final_result.size());
  output_file.close();

  return 0;
}

int main (int argc, char* argv[]) {

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <command> [args...]" << std::endl;
    std::cerr << "Commands:" << std::endl;
    std::cerr << "  vocab <corpus_file>                 - Generate vocabulary from corpus" << std::endl;
    std::cerr << "  encode <input> <vocab> <output>     - Encode text using vocabulary" << std::endl;
    std::cerr << "  decode <input> <vocab> <output>     - Decode tokens to text" << std::endl;
    return 1;
  }

  std::string command = argv[1];

  if (command == "vocab") {
    if (argc != 3) {
      std::cerr << "Usage: " << argv[0] << " vocab <corpus_file>" << std::endl;
      return 1;
    }
    return generate_vocabulary(argv[2]);
  } 
  else if (command == "encode") {
    if (argc != 5) {
      std::cerr << "Usage: " << argv[0] << " encode <input> <vocab> <output>" << std::endl;
      return 1;
    }
    return encode(argv[2], argv[3], argv[4]);
  } 
  else if (command == "decode") {
    if (argc != 5) {
      std::cerr << "Usage: " << argv[0] << " decode <input> <vocab> <output>" << std::endl;
      return 1;
    }
    return decode(argv[2], argv[3], argv[4]);
  } 
  else {
    std::cerr << "Unknown command: " << command << std::endl;
    return 1;
  }

  return 0;
}