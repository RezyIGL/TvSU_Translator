#include <iostream>
#include <sstream>
#include "ll.h"

int main() {
	std::vector<std::string> inputs = {
		"5 + 5 -",
		"5 + 5 - 5 * 5 || (a + (b * c) && 2)",
		"(a + 2"
	};

	for (auto inp : inputs) {
		std::stringstream ss(inp);
		LL ll = LL(ss);
		std::cout << "============================================================================================" << std::endl;
		std::cout << "Expression: " << inp << std::endl;
		std::cout << "Result: " << ll.validate() << std::endl;
		std::cout << "============================================================================================\n" << std::endl;
	}
}
