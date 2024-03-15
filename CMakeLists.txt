cmake_minimum_required(VERSION 3.27)
project(TvSU-Translator)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(LexerLib)
add_subdirectory(ShuntingYardLib)
add_subdirectory(Google_test)

add_executable(Lexer_run main.cpp)

target_link_libraries(Lexer_run LexerLib ShuntingYardLib)
