cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(
  bpe
  VERSION 0.1
  DESCRIPTION "Implementation of Byte-Pair Encoding tokenization for Large Language Models"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED_ON)

add_executable(bpe ./src/main.cpp)

target_compile_features(bpe PRIVATE cxx_std_17)
target_compile_options(bpe PRIVATE -w)