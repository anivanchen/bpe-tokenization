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

int generate_vocabulary(char* filename, int ITERATION_LIMIT) {
  
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
    characters.push_back("<>");

    for (int j = 0; j < word->first.length(); j++) {
      characters.push_back(std::string(1, word->first[j]));
    }

    split_strings.push_back(characters);
    word_counts.push_back(word->second);
  }

  // Initialize the vocabulary with unique characters

  std::unordered_map<std::string, int> vocabulary;

  for (int i = 0; i < split_strings.size(); i++) {
    for (int j = 0; j < split_strings[i].size(); j++) {
      vocabulary[split_strings[i][j]] += word_counts[i];
    }
  }

  // for (const auto& entry : vocabulary) {
  //   std::cout << "Word: " << entry.first << ", Count: " << entry.second << std::endl;
  // }

  int iteration = 0;

  while (iteration++ < ITERATION_LIMIT) { // maybe replace this with a target vocabulary size (32000k)
    
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

    std::cout << "Max Val: " << max_val << " , Max Key: " << max_key << std::endl;
    
    // Merge all instances of the most frequent bigram

    auto start = std::chrono::high_resolution_clock::now();

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

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;
    std::cout << "aElapsed time: " << elapsed.count() << " s\n";

    // auto start = std::chrono::high_resolution_clock::now();

    // for (int i = 0; i < split_strings.size(); i++) {
    //   for (int j = 0; j < split_strings[i].size() - 1; j++) {
    //     std::string a = split_strings[i][j];
    //     std::string b = split_strings[i][j + 1];
    //     std::string ab = a + b;

    //     if (ab == max_key) {
    //       split_strings[i][j] = ab;
    //       split_strings[i].erase(split_strings[i].begin() + j + 1);

    //       if (vocabulary.find(ab) == vocabulary.end()) {
    //         vocabulary[ab] = word_counts[i];
    //       }

    //       vocabulary[a]  -= word_counts[i];
    //       vocabulary[b]  -= word_counts[i];
    //       vocabulary[ab] += word_counts[i];
    //     }
    //   }
    // }

    // auto end = std::chrono::high_resolution_clock::now();
    // std::chrono::duration<double> elapsed = end - start;
    // std::cout << "bElapsed time: " << elapsed.count() << " s\n";

  }

  // Write vocabulary to file for storage

  return 0;
}

int encode(char* input_filename, char* vocabulary_filename, char* output_filename) {
  
}
           
int decode(char* input_filename, char* vocabulary_filename, char* output_filename) {

}

int main (int argc, char* argv[]) {

  generate_vocabulary("../.testfiles/war-and-peace.txt", 2048);

  return 0;
}