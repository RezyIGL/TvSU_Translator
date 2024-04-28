cmake_minimum_required(VERSION 3.27)
project(TvSU-Translator)

set(CMAKE_CXX_STANDARD 20)

add_subdirectory(LexerLib)
add_subdirectory(Translator)
add_subdirectory(Google_test)

add_executable(run_exe main.cpp)

target_link_libraries(run_exe LexerLib Translator)
