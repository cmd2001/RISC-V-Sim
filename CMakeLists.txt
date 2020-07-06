cmake_minimum_required (VERSION 2.6)
add_compile_options(-std=c++11 -Wextra -Ofast)
project (RISC-V)
add_executable(code main.cpp)