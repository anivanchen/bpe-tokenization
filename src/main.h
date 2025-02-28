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

std::vector<char> rftv(char* filename);

std::vector<std::string> read_vocab(char* vocab_filename);

int generate_vocabulary(char* filename);

int encode(char* input_filename, 
           char* vocabulary_filename, 
           char* output_filename);

int decode(char* input_filename, 
           char* vocabulary_filename, 
           char* output_filename);
#endif