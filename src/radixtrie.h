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
        // Match as many characters as possible with the child's key
        while (j < child->key.size() && i + j < word.size() && child->key[j] == word[i + j]) j++;

        if (j == child->key.size()) {  // If we matched the entire key
          node = child;  // Move to child
          i += j;  // Skip matched portion in word
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
          break;  // Encountered mismatch, stop here
      } else
        break;  // No matching child for current character
    }

    return longest_match;
  }

  void insert(const std::string& word) {
    if (word.empty()) return;

    Node* node = root;
    size_t i = 0;
    while (i < word.size()) {
      char current_char = word[i];

      if (node->children.find(current_char) != node->children.end()) {
        Node* child = node->children[current_char];

        // Find common prefix
        size_t j = 0;
        while (j < child->key.size() && i + j < word.size() && child->key[j] == word[i + j]) j++;

        if (j == child->key.size()) {
          node = child;  // Move to child and continue with next part of word
          i += j;
        } else {
          // Split node at common prefix
          std::string common_prefix = child->key.substr(0, j);
          std::string remaining_child_key = child->key.substr(j);
          std::string remaining_word = word.substr(i + j);

          // Create new intermediate node for common prefix
          Node* new_child = new Node(common_prefix);
          node->children[current_char] = new_child;

          // Create old child for remainder of old key
          Node* old_child = new Node(remaining_child_key);
          new_child->children[remaining_child_key[0]] = old_child;

          // Set child as an end of word if needed
          if (old_child->isEnd) old_child->isEnd = false;

          // Insert remaining word from new node
          insert(remaining_word);
          return;
        }
      } else {
        // No matching child, create new node
        Node* new_node = new Node(std::string(1, current_char));
        node->children[current_char] = new_node;
        node = new_node;
        i++;
      }
    }

    // Mark end of word
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