#include "ll.h"
#include <fstream>
#include <iostream>

int main() {
    std::string output = "../output.graph";
	std::string atomsOutput = "../result.atom";
	std::fstream input("../input.minic");

	LL ll(input, output, atomsOutput);
	ll.validate();
}
