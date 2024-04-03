#include <iostream>
#include <fstream>
#include "ll.h"

int main() {
	std::fstream streamline(R"(E:\CLionProjects\TvSU_Translator\miniC-code.txt)");
	LL ll(streamline);
	std::cout << ll.validate() << std::endl;
}
