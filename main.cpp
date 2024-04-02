#include <iostream>
#include <sstream>
#include "ll.h"

int main() {
	std::vector<std::string> inputs = {
		"5 + 5",
		"5 + 5 -",
		"5 + 5 - 5 * 5 || (a + (b * c) && 2)",
		"(a + 2",
		"1 || 2 && 3 == 4 != 5 < 6 + 7 - 8 * 9 !10",
		"1 || 2 && 3 == 4 != 5 < 6 + 7 - 8 * !9++",
		"1 || 2 && 3 == 4 != 5 < 6 + 7 - 8 * !9++ + (a)",
		"(1 + 2)*(3 + 4)",
		"1*2*3*4",
		"1+ 2*2"
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
