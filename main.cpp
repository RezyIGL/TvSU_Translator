#include <iostream>
#include <fstream>
#include "ll.h"

int main() {
	std::fstream input(R"(E:\CLionProjects\TvSU_Translator\miniC-code.txt)");
	LL ll(input);
	ll.validate();

	return 0;
}
