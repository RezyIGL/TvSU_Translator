#include "ll.h"

int main() {
	std::fstream INPUT_PATH("../MiniC_Code.minic");
	const std::string GRAPH_PATH = "../CodeGraph.graph";
	const std::string ATOMS_PATH = "../Atoms.atom";
	const std::string ASM_PATH = "../Result.asm";

	LL ll(INPUT_PATH, GRAPH_PATH, ATOMS_PATH, ASM_PATH);

	ll.validate();
}
