#include "main.h"

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

  // for (const auto& entry : unique_words) {
  //   std::cout << "Word: " << entry.first << ", Count: " << entry.second << std::endl;
  // }

  // Split words into characters, mark ends with </w>, 
  std::vector<std::vector<std::string>> split_strings;
  std::vector<int> word_counts;

  for (auto word = unique_words.begin(); word != unique_words.end(); word++) {
    std::vector<std::string> characters;
    characters.push_back("</w>");

    for (int j = 0; j < word->first.length(); j++) {
      characters.push_back(std::string(1, word->first[j]));
    }

    split_strings.push_back(characters);
    word_counts.push_back(word->second);
  }

  return 0;
}

int encode(char* input_filename, char* vocabulary_filename, char* output_filename) {
  
}
           
int decode(char* input_filename, char* vocabulary_filename, char* output_filename) {

}

int main (int argc, char* argv[]) {

  generate_vocabulary("../.testfiles/war-and-peace.txt", 1);

  return 0;
}