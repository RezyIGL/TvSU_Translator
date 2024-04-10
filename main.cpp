#include "ll.h"
#include <fstream>

int main() {
    std::string output = R"(E:\CLionProjects\TvSU_Translator\output.txt)";
    std::fstream input(R"(E:\CLionProjects\TvSU_Translator\miniC-code.txt)");

	/*
	 * Currently you can write:
	 *
	 * StmtList;
	 * Stmt (partially);
	 * E;
	 * ';';
	 * {StmtList};
	 * return E;
	 */

	LL ll(input, output);
	ll.validate();

	return 0;
}
