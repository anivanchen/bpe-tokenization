# Byte-Pair Encoding Tokenization

Byte Pair Encoding (BPE) is a data compression and subword tokenization algorithm used to segment text into subword units. It iteratively merges the most frequent pair of adjacent bytes until no further merges are possible, an iteration limit has been reached, or a desired vocabulary size has been reached. This results in a vocabulary, a set of unique tokens that efficiently represent text data. This makes it particularly useful for:

- Neural language models
- Machine translation systems
- Text compression applications
- Balancing vocabulary size with the ability to represent rare words as sequences of subword units

> Note: This is an educational implementation.

## Implementation Features

#### Core Components
- Vocabulary Generation: Builds a subword vocabulary by iteratively merging the most frequent character pairs
- Radix Trie: Efficiently stores the vocabulary for fast prefix matching
- Encoder: Converts raw text into token sequences using the vocabulary
- Decoder: Reconstructs original text from token sequences

#### Key Optimizations
- Radix Trie for Prefix Matching: Uses a space-efficient trie structure for fast longest-prefix lookups.
- Caching System: Implements a prefix cache to avoid redundant lookups during tokenization.
- Base94 Tokens: Implements a base10 to base94 conversion for more compressed tokens.
- Strings/Maps: Minimized lookups and reconstruction of strings. Skips `<>` in decoding instead of `str.replace()`. Use `string_view` for string operations.

## Dependencies

- C++17 compatible compiler
- CMake 3.30 or higher
- Standard C++ libraries only (no external dependencies)

## Usage

#### Building the Vocabulary
This generates a vocabulary file (vocabulary.tokens) from your training corpus.

```bash
./bpetok vocab <corpus_file>
```

#### Encoding Text
Converts plain text into a sequence of tokens based on the vocabulary.

```bash
./bpetok encode <input_file> <vocabulary_file> <output_file>
```

#### Decoding Text
Reconstructs the original text from token sequences.

```bash
./bpetok decode <input_file> <vocabulary_file> <output_file>
```

#### Building from Source

```bash
mkdir build && cd build
cmake ..
make
```

## Implementation Details

#### Vocabulary Generation
1. Process corpus into unique words, count them, then split each word into characters.
2. Initializes vocabulary with all printable ASCII characters.
3. Counts frequency of character bigrams in the corpus.
4. Iteratively merges the most frequent pairs.
5. Continues until reaching target vocabulary size (50,000 tokens) or no more bigrams can be found.

#### Tokenization Process
1. Reads input and vocabulary files.
2. Normalizes some UTF-8 characters and handles special cases like smart quotes.
3. Build a Radix Trie using the vocabulary data.
4. Splits input text on whitespace
5. For each word:
    1. Appends end-of-word marker (<>)
    2. Finds longest matching subword from vocabulary using the Radix Trie
    3. Repeats until the entire word is tokenized
6. Writes tokenized data into file

#### Detokenization Process
1. Reads input and vocabulary files.
2. Use a stream to grab tokens split by whitespace.
3. Lookup token in vocabulary map.
4. Slice off delimiters and output to file.

## License
This codebase is licensed under the terms of the GNU General Public License v3.0 - see the [LICENSE](LICENSE) file for details.