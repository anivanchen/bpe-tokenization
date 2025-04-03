/*******************************************************************************
 * Byte-Pair Encoding (BPE) Tokenizer Header
 * 
 * Header file defining the core interfaces for the BPE tokenizer implementation.
 * This file contains function declarations for the tokenizer's main operations
 * and utility functions.
 * 
 * Author: Ivan Chen <ivanchen07@gmail.com>
 * Created: March 2025
 * Version: 1.0
 * 
 * Declarations include:
 * - Core tokenizer functions (generate_vocabulary, encode, decode)
 * - Utility functions for string and token handling
 * - File I/O operations for vocabulary and text processing
 * 
 * Dependencies:
 * - C++ Standard Library
 * - RadixTrie data structure (radixtrie.h)
 * 
 * License: GNU General Public License v3.0
 *******************************************************************************/

#pragma once
#ifndef __MAIN_H__
#define __MAIN_H__

#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

#include "radixtrie.h"

bool is_symb(char c);
void reverseString(std::string& str);
std::string getTokRep(int n);

std::vector<char> rftv(char* filename);
std::unordered_map<std::string, std::string> read_vocab(char* vocab_filename, bool tokenLookup);

int generate_vocabulary(char* filename);

int encode(char* input_filename, char* vocabulary_filename, char* output_filename);
int decode(char* input_filename, char* vocabulary_filename, char* output_filename);

#endif