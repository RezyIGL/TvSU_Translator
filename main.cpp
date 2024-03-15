#include <fstream>
#include <iostream>
#include "SYM.h"

int main() {
	std::fstream stream(R"(E:\CLionProjects\LexerTvSU\CodeToAnalyze.txt)");

	ShuntingYard shYard(stream);

	std::pair<std::string, std::string> lexem;

	while (true) {
		lexem = shYard.getNextLexem();
		std::cout << lexem.first << " " << lexem.second << std::endl;

		if (lexem.first == "eof" || lexem.first == "error") {
			break;
		}
	}
}
