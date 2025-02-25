#pragma once 
#ifndef __MAIN_H__
#define __MAIN_H__

#include <cstring>
#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

#include <chrono>

std::vector<char> rftv(char* filename);

int generate_vocabulary(char* filename, int ITERATION_LIMIT);

int encode(char* input_filename, 
           char* vocabulary_filename, 
           char* output_filename);

int decode(char* input_filename, 
           char* vocabulary_filename, 
           char* output_filename);
#endif