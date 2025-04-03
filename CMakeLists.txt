cmake_minimum_required(VERSION 3.30 FATAL_ERROR)

project(
  bpe
  VERSION 1.0
  DESCRIPTION "Implementation of Byte-Pair Encoding tokenization"
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED_ON)

add_executable(bpe ./src/main.cpp)

target_compile_features(bpe PRIVATE cxx_std_17)
target_compile_options(bpe PRIVATE -w)