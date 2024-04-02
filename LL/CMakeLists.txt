cmake_minimum_required(VERSION 3.27)
project(LL)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_library(LL src/ll.cpp)
target_link_libraries(LL LexerLib)
target_include_directories(LL PUBLIC include)
