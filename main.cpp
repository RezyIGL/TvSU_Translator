#include "Translator.h"
#include "iomanip"

int main() {
    // Path to file that contains miniC code.
	std::fstream INPUT_PATH("../files/MiniC_Code.minic");

    // Path to file that contains your graph of decompose.
	const std::string GRAPH_PATH = "../files/CodeGraph.graph";

    // Path to file that contains Atoms of your miniC code.
	const std::string ATOMS_PATH = "../files/Atoms.atom";

    // Path to file that contains ASM i8080 code.
	const std::string ASM_PATH = "../files/Result.asm";

    // Translator object to do translation
	LL Translator(INPUT_PATH, GRAPH_PATH, ATOMS_PATH, ASM_PATH);

    // Says whether your miniC code contains error or not.
    Translator.checkIfCodeSyntaxIsCorrect();
    // Generates graph of decompose into GRAPH_PATH
    Translator.printOutputGraph();
    // Generates Atoms of your miniC code into ATOMS_PATH
    Translator.printAtoms();
    // Generates ASM i8080 code into ASM_PATH
    Translator.printASMCode();
}
