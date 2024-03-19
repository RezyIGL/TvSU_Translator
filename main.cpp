#include <iostream>
#include <sstream>
#include "SYM.h"

int main() {
	std::string input = "12 + 3";

	std::stringstream stream { input };
	ShuntingYard shYard(stream);

	Lexem lexem;

	while (true) {
		lexem = shYard.getNextLexem();
		std::cout << "[\"" << lexem.first << "\", \"" << lexem.second << "\"]" << std::endl;

		if (lexem.first == "eof" || lexem.first == "error") {
			break;
		}
	}
}
