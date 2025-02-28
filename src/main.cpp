#include "main.h"

bool is_symb(char c) {
  return !std::isalnum(c) && !std::isspace(c) && c != '<' && c != '>';
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

int generate_vocabulary(char* filename) {
  
  // Read corpus file into vector
  std::vector<char> raw_text = rftv(filename);

  // Build dictionary of unique words and count frequency

  std::unordered_map<std::string, int> unique_words;

  char* word = strtok(raw_text.data(), " \t\n");

  while (word) {
    unique_words[word]++;
    word = strtok(NULL, " \t\n");
  }

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

  std::unordered_map<std::string, int> vocabulary;
  std::vector<std::string> ins_ord_vocab;

  for (int i = 0; i < split_strings.size(); i++) {
    for (int j = 0; j < split_strings[i].size(); j++) {
      vocabulary[split_strings[i][j]] += word_counts[i];
    }
  }

  for (const auto& vocab: vocabulary) {
    ins_ord_vocab.push_back(vocab.first);
  }

  int iter = 0;

  while (vocabulary.size() < 32000) {
    
    std::unordered_map<std::string, int> bigrams;

    // Find bigrams of characters in split_words and counts of bigrams
    // Track the most frequent bigram

    int max_val = 0;
    std::string max_key = "";

    for (int i = 0; i < split_strings.size(); i++) {
      for (int j = 0; j < split_strings[i].size() - 1; j++) {
        if (is_symb(*(split_strings[i][j].c_str())) || is_symb(*(split_strings[i][j+1].c_str()))) continue;
        std::string ab = split_strings[i][j] + split_strings[i][j+1];
        bigrams[ab] += word_counts[i];

        if (bigrams[ab] > max_val) {
          max_val = bigrams[ab];
          max_key = ab;
        }
      }
    }

    if (bigrams.empty()) break;

    ins_ord_vocab.push_back(max_key);
    
    // Merge all instances of the most frequent bigram

    for (int i = 0; i < split_strings.size(); i++) {
      for (int j = 0; j < split_strings[i].size() - 1; j++) {
        std::string ab = split_strings[i][j] + split_strings[i][j + 1];

        if (ab == max_key) {
          split_strings[i][j] = ab;
          split_strings[i].erase(split_strings[i].begin() + j + 1);

          vocabulary[std::string(1, ab.at(0))] -= word_counts[i];
          vocabulary[std::string(1, ab.at(1))] -= word_counts[i];
          vocabulary[ab] += word_counts[i];
        }
      }
    }

    std::cout << "(" << iter++ << ") " << max_key << std::endl; 

  }

  // Write vocabulary to file for storage

  std::ofstream output_file("vocabulary.tokens");

  int n = 0;
  for (std::string s : ins_ord_vocab) {
    output_file << s << " " << n++ << std::endl;
  }

  output_file.close();

  return 0;
}

std::vector<std::string> read_vocab(char* vocab_filename) {
  
  std::vector<std::string> vocab;
  
  std::ifstream file(vocab_filename);

  if (!file.is_open()) throw std::runtime_error("Could not open file");

  std::string line;

  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::string word, token;

    if (iss >> word >> token) vocab.push_back(word);
  }

  file.close();

  return vocab;
}

bool startsWith(const std::string str, const std::string prefix) {
  return str.compare(0, prefix.size(), prefix) == 0;
}

int encode(char* input_filename, char* vocabulary_filename, char* output_filename) {
  std::vector<char> input_data = rftv(input_filename);
  std::vector<std::string> vocab = read_vocab(vocabulary_filename);
  std::unordered_map<std::string, int> vocab_map;

  int n = 0;
  for (std::string word : vocab) {
    vocab_map[word] = n++;
  }

  std::string str(input_data.begin(), input_data.end());
  std::vector<std::string> words;
  std::string word;

  std::istringstream stream(str);
  while(stream >> word) {
    words.push_back("<>" + word);
  }

  std::vector<std::vector<std::string>> subwords(words.size());
  std::vector<int> tokens;

  std::unordered_map<std::string, std::string> prefix_cache;

  // Iterate over each word in the words vector
  for (int i = 0; i < words.size(); i++) {

    // Continue processing the word until it is empty
    while (words[i].length() > 0) {
      std::string max_subword;

      // Find the longest subword in the vocabulary that matches the beginning of the current word

      // Check if the longest subword is already cached
      if (prefix_cache.find(words[i]) != prefix_cache.end()) {
        max_subword = prefix_cache[words[i]];
      } else {
        // If not cached, find the longest subword using vocab
      for (int j = 0; j < vocab.size(); j++) {
        if (vocab[j].length() > max_subword.length()) {
          if (startsWith(words[i], vocab[j])) max_subword = vocab[j];
        }
        }

        // Cache the result for future lookups
        prefix_cache[words[i]] = max_subword;
      }

      if (!max_subword.empty()) {
        // If a subword is found, add it to the subwords list and remove it from the current word
        subwords[i].push_back(max_subword);
        words[i] = words[i].substr(max_subword.length()); // chop off subword
        tokens.push_back(vocab_map[max_subword]);
      } else {
        // If no subword is found, add the first character as a subword
        subwords[i].push_back(std::string(1, words[i][0]));
        words[i] = words[i].substr(1);
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
  std::vector<std::string> vocab = read_vocab(vocabulary_filename);

  // Create a map from vocabulary words to their index
  std::unordered_map<std::string, int> vocab_map;
  int n = 0;
  for (const auto& word : vocab) {
    vocab_map[word] = n++;
  }

  // Convert input data to a string
  std::string str(input_data.begin(), input_data.end());
  std::vector<char> result;
  result.reserve(str.size());  // Reserve enough space for the result string

  std::string word;
  std::istringstream stream(str);
  while (stream >> word) {
      // Get the word's index from the vocab_map and append the corresponding word
      int word_index = std::stoi(word);
      result.insert(result.end(), vocab[word_index].begin(), vocab[word_index].end());
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

  generate_vocabulary("../.testfiles/war-and-peace.txt", 2048);

  return 0;
}