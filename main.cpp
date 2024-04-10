#include "ll.h"
#include <fstream>

int main() {
 // TODO: Pass inputFile name to LL constructor
    std::string output = R"(E:\CLionProjects\TvSU_Translator\output.txt)";
    std::fstream input(R"(E:\CLionProjects\TvSU_Translator\miniC-code.txt)");

	LL ll(input, output);
	ll.validate();

	return 0;
}
