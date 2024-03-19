cmake_minimum_required(VERSION 3.27)
project(LALR)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_library(LALR src/)
target_include_directories(LALR PUBLIC include)