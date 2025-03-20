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
      if (node->children.find(current_char) != node->children.end()) {
        Node* child = node->children[current_char];

        size_t j = 0;
        while (j < child->key.size() && i + j < word.size() && child->key[j] == word[i + j]) j++;

        if (j == child->key.size()) {
          node = child;
          i += j;
        } else
          return false;
      } else
        return false;
    }
    return node->isEnd;
  }

  std::string longest_prefix(const std::string& word) {
    if (word.empty()) return "";

    Node* node = root;
    std::string longest_match;

    size_t i = 0;
    while (i < word.size()) {
      char current_char = word[i];

      // Check if current character exists in node's children
      if (node->children.find(current_char) != node->children.end()) {
        Node* child = node->children[current_char];

        // Match prefix as far as possible
        size_t j = 0;
        while (j < child->key.size() && i + j < word.size() && child->key[j] == word[i + j]) j++;

        if (j == child->key.size()) {  // We matched entire key of child
          longest_match += child->key;
          node = child;  // Move to child
          i += j;        // Skip matched portion in word
        } else
          break;  // Encountered  mismatch, stop here
      } else
        break;  // No matching child for current character
    }

    return longest_match;
  }

  // Utility function to print the Radix Trie
  void print() { printHelper(root, ""); }
};

#endif