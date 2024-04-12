#include "ll.h"
#include <fstream>

int main() {
    std::string output = "../output.txt";
    std::fstream input("../input.minic");

	LL ll(input, output);
	ll.validate();

	return 0;
}
