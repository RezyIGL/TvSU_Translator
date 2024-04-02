#include <iostream>
#include <sstream>
#include "ll.h"

int main() {
	std::string input = "5 + 5 + 5 - 5 * (a + b) || a - 2";
	std::stringstream ss(input);

	LL ll = LL(ss);
	std::cout << ll.validate() << std::endl;
}
