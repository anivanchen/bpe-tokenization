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

  // Utility function to print the Radix Trie
  void print() { printHelper(root, ""); }
};

#endif