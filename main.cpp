#include <iostream>
#include <sstream>
#include "ll.h"

int main() {
	std::vector<std::string> inputs = {
		"int a;",
		"int a = 1;",
		"int a, b;",
		"int a = 2, b;",
		"int a, b = 3;",
		"int a, b, c;",
		"int a, b = 4, c;",
		"int a, b, c = 5;",
		"char a = '5', b, c = '5';",
		"int a = 5;\nchar c = '5';",
	};

	for (const auto &inp : inputs) {
		std::stringstream ss(inp);
		LL ll = LL(ss);
		using namespace std;
		cout << "============================================================================================" << endl;
		cout << "Expression: " << inp << endl;
		cout << "Result: " << ll.validate() << endl;
		cout << "============================================================================================\n" << endl;
	}
}
