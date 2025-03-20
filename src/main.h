#pragma once 
#ifndef __MAIN_H__
#define __MAIN_H__

#include <cctype>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <chrono>

#include "radixtrie.h"

bool is_symb(char c);
bool startsWith(const std::string str, const std::string prefix);
void reverseString(std::string &str);
std::string getTokRep(int n);

std::vector<char> rftv(char* filename);
std::unordered_map<std::string, std::string> read_vocab(char* vocab_filename, bool tokenLookup);

int generate_vocabulary(char* filename);

int encode(char* input_filename, 
           char* vocabulary_filename, 
           char* output_filename);

int decode(char* input_filename, 
           char* vocabulary_filename, 
           char* output_filename);
#endif