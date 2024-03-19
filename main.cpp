#include <iostream>
#include <sstream>
#include "SYM.h"

int main() {
	std::string input = "12 + 3";

	std::stringstream stream { input };
	ShuntingYard shYard(stream);

	Token Token;

	while (true) {
		Token = shYard.getNextToken();
		std::cout << "[\"" << Token.first << "\", \"" << Token.second << "\"]" << std::endl;

		if (Token.first == "eof" || Token.first == "error") {
			break;
		}
	}
}
