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

#endif