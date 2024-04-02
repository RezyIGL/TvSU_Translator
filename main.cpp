#include <iostream>
#include <sstream>
#include "ll.h"

int main() {
	std::vector<std::string> inputs = {
		"5 + 5", // Correct
		"5 + 5 -", // Incorrect
		"5 + 5 - 5 * 5 || (a + (b * c) && 2)", // Correct
		"(a + 2", // Incorrect
		"1 || 2 && 3 == 4 != 5 < 6 + 7 - 8 * 9 !10", // Incorrect
		"1 || 2 && 3 == 4 != 5 < 6 + 7 - 8 * !9++", // Incorrect
		"1 || 2 && 3 == 4 != 5 < 6 + 7 - 8 * !9++ + (a)", // Incorrect
		"(1 + 2)*(3 + 4)", // Correct
		"1*2*3*4", // Correct
		"1+ 2*2" // Correct
	};

	for (const auto &inp : inputs) {
		std::stringstream ss(inp);
		LL ll = LL(ss);
		std::cout << "============================================================================================" << std::endl;
		std::cout << "Expression: " << inp << std::endl;
		std::cout << "Result: " << ll.validate() << std::endl;
		std::cout << "============================================================================================\n" << std::endl;
	}
}
