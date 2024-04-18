#include "ll.h"
#include <fstream>
#include <iostream>

int main() {
    std::string output = "../output.graph";
    std::fstream input("../input.minic");
	std::string atomsOutput = "../result.atom";

	LL ll(input, output, atomsOutput);
	ll.validate();
}
