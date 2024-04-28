#include "Translator.h"

int main() {
	std::fstream INPUT_PATH("../files/MiniC_Code.minic");
	const std::string GRAPH_PATH = "../files/CodeGraph.graph";
	const std::string ATOMS_PATH = "../files/Atoms.atom";
	const std::string ASM_PATH = "../files/Result.asm";

	LL Translator(INPUT_PATH, GRAPH_PATH, ATOMS_PATH, ASM_PATH);

	Translator.checkIfCodeSyntaxIsCorrect();
	Translator.printOutputGraph();
	Translator.printAtoms();
	Translator.printASMCode();
}
