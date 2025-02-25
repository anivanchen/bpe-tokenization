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

  // Build dictionary of unique words

  std::unordered_map<std::string, int> unique_words;

  char* word = strtok(raw_text.data(), " \t\n");

  while (word) {
    if (unique_words.find(word) != unique_words.end()) {
      unique_words[word] += 1;
    }
    else {
      unique_words[word] = 1;
    }

    word = strtok(NULL, " \t\n");
  }

  for (const auto& entry : unique_words) {
    std::cout << "Word: " << entry.first << ", Count: " << entry.second << std::endl;
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