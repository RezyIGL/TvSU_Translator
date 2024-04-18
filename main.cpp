#include "ll.h"
#include <fstream>
#include <iostream>

int main() {
    std::string output = "../output.graph";
    std::fstream input("../input.minic");
	std::string atomsOutput = "../result.atom";

	LL ll(input, output, atomsOutput);
	try {
		ll.validate();
	} catch (std::exception &e) {
		std::cerr << e.what() << std::endl;
		return -1;
	}

	return 0;
}
