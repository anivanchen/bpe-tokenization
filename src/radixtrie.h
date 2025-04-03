/*******************************************************************************
 * RadixTrie Implementation for BPE Tokenizer
 * 
 * A space and time-efficient implementation of a Radix Trie data
 * structure for subword tokenization in the BPE algorithm.
 * 
 * Author: Ivan Chen <ivanchen07@gmail.com>
 * Created: March 2025
 * Version: 1.0
 * 
 * This implementation features:
 * - Efficient longest prefix matching for tokenization
 * - Optimized memory usage with shared prefixes
 * - Fast lookup operations using unordered_map for child nodes
 * - Support for insert, search, and remove operations
 * - Bulk character comparison using std::mismatch for performance
 * 
 * The RadixTrie enables fast lookups during the tokenization process,
 * significantly improving encoding performance compared to brute-force
 * string matching.
 * 
 * License: GNU General Public License v3.0
 *******************************************************************************/

#pragma once
#ifndef __RADIX_TRIE_H__
#define __RADIX_TRIE_H__

#include "main.h"

class Node {
 public:
  std::string key;
  std::unordered_map<char, Node*> children;
  bool isEnd;

  Node(std::string key) : key(key), isEnd(false) {}
  Node() : key(""), isEnd(false) {}
};

class RadixTrie {
 private:
  Node* root;

  // Recursively delete child nodes
  void deleteSubTree(Node* node) {
    if (node == nullptr) return;
    for (auto& child : node->children) deleteSubTree(child.second);
    delete node;
  }

  // Print the Trie recursively
  void printHelper(Node* node, const std::string& prefix) {
    if (node->isEnd) std::cout << prefix + " " + node->key << std::endl;
    for (auto& child : node->children) printHelper(child.second, prefix + child.first);
  }

 public:
  RadixTrie() { root = new Node(); }
  ~RadixTrie() { deleteSubTree(root); }

  bool search(const std::string& word) {
    Node* node = root;

    size_t i = 0;
    while (i < word.size()) {
      char current_char = word[i];
      auto it = node->children.find(current_char);
      if (it != node->children.end()) {
        Node* child = it->second;

        size_t j = 0;
        // Match as many characters as possible with the child's key
        while (j < child->key.size() && i + j < word.size() && child->key[j] == word[i + j]) j++;

        if (j == child->key.size()) {  // If we matched the entire key
          node = child;                // Move to child
          i += j;                      // Skip matched portion in word
        } else
          return false;  // Partial match - word doesn't exist
      } else
        return false;  // Character not found - word doesn't exist
    }
    return node->isEnd;
  }

  std::string longest_prefix(const std::string& word) {
    if (word.empty()) return "";

    Node* node = root;
    std::string longest_match;
    size_t i = 0;
    while (i < word.size()) {
      if (node->isEnd) longest_match = word.substr(0, i);  // Update longest match when we find a complete word

      auto it = node->children.find(word[i]);
      if (it == node->children.end()) break;

      Node* child = it->second;

      auto mismatch_pos = std::mismatch(child->key.begin(), child->key.end(), word.begin() + i);
      size_t j = std::distance(child->key.begin(), mismatch_pos.first);

      if (j == child->key.size()) {  // Fully matched child key
        node = child;
        i += j;
      } else
        break;
    }

    // Check one more time in case last node is end of word
    if (node->isEnd) longest_match = word.substr(0, i);

    return longest_match;
  }

  void insert(const std::string& word) {
    if (word.empty()) return;

    Node* node = root;
    size_t i = 0;
    while (i < word.size()) {
      char current_char = word[i];

      // If no child, create new branch for rest of word
      if (node->children.find(current_char) == node->children.end()) {
        Node* new_node = new Node(word.substr(i));
        new_node->isEnd = true;
        node->children[current_char] = new_node;
        return;
      }

      Node* child = node->children[current_char];
      size_t j = 0;
      // Find common prefix between child->key and word starting at i
      while (j < child->key.size() && i + j < word.size() && child->key[j] == word[i + j]) j++;

      // Full match: continue traversal
      if (j == child->key.size()) {
        node = child;
        i += j;
      } else {
        // Partial match: need to split child
        std::string commonPrefix = child->key.substr(0, j);
        std::string childRemaining = child->key.substr(j);
        std::string wordRemaining = word.substr(i + j);

        // Create new intermediate node with common prefix
        Node* intermediate = new Node(commonPrefix);
        node->children[current_char] = intermediate;

        // Adjust old child
        child->key = childRemaining;
        intermediate->children[childRemaining[0]] = child;

        // If there is remaining part in inserted word, add as new child
        if (!wordRemaining.empty()) {
          Node* newChild = new Node(wordRemaining);
          newChild->isEnd = true;
          intermediate->children[wordRemaining[0]] = newChild;
        } else {
          intermediate->isEnd = true;  // Otherwise, intermediate node marks end of word
        }
        return;
      }
    }

    // Mark last node as end of word
    node->isEnd = true;
  }

  bool remove(Node* node, const std::string& word, size_t index = 0) {
    if (index == word.size()) {
      // We have reached the end of the word
      if (!node->isEnd) {
        return false;  // Word does not exist, nothing to remove
      }
      node->isEnd = false;  // Mark the node as not the end of a word

      // If node has no children, it can be deleted
      return node->children.empty();
    }

    char current_char = word[index];
    if (node->children.find(current_char) == node->children.end()) {
      // If the character doesn't exist in the current node's children, word doesn't exist
      return false;
    }

    Node* child = node->children[current_char];

    // Recursively remove the word from the child node
    bool should_delete_child = remove(child, word, index + 1);

    // If the child node should be deleted (i.e., it's no longer needed)
    if (should_delete_child) {
      node->children.erase(current_char);  // Remove the child node from the current node
      // If the current node is not the end of another word and has no children, it can also be deleted
      return node->children.empty() && !node->isEnd;
    }

    return false;  // If we don't need to delete the node, return false
  }

  void remove(const std::string& word) { remove(root, word); }

  // Utility function to print the Radix Trie
  void print() { printHelper(root, ""); }
};

#endif