cmake_minimum_required(VERSION 3.27)
project(ShuntingYardLib)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_library(ShuntingYardLib src/SYM.cpp)
target_link_libraries(ShuntingYardLib LexerLib)
target_include_directories(ShuntingYardLib PUBLIC include)
