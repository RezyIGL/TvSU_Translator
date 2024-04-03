#include <iostream>
#include <sstream>
#include <fstream>
#include "ll.h"

int main() {
//	std::vector<std::string> inputs = {
//		"int main() {int a = 3; char c = '5'; a = 2 + c; in b; out 13 + 2; return 4;}",
//	};
//
//	for (const auto &inp : inputs) {
//		std::stringstream ss(inp);
//		LL ll = LL(ss);
//		using namespace std;
//		cout << "============================================================================================" << endl;
//		cout << "Expression: " << inp << endl;
//		cout << "Result: " << ll.validate() << endl;
//		cout << "============================================================================================\n" << endl;
//	}

	std::fstream streamline(R"(E:\CLionProjects\TvSU_Translator\miniC-code.txt)");
	LL ll(streamline);
	std::cout << ll.validate() << std::endl;
}
