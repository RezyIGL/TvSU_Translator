cmake_minimum_required(VERSION 3.27)
project(Translator)

set(CMAKE_CXX_STANDARD 20)

include_directories(include)

add_library(Translator src/Translator.cpp src/ASMPrint.cpp src/AtomsPrint.cpp src/ExprGrammar.cpp src/LangGrammar.cpp src/Functions.cpp
)
target_link_libraries(Translator LexerLib)
target_include_directories(Translator PUBLIC include)
