#include "Translator.h"
#include <ranges>

LL::LL(std::istream &stream,
	   const std::string &Graph = "",
	   const std::string &Atoms = "",
	   const std::string &Asm = "") : lexer{stream} {

	_graph = Graph;
	_atomsInput = Atoms;
	_asmOut = Asm;

	clearStream(_graph);
	clearStream(_atomsInput);
	clearStream(_asmOut);
}

// Function to say if your MiniC code contains Error
bool LL::checkIfCodeSyntaxIsCorrect() {

	std::cout << "=====================" << std::endl;
	std::cout << "   Checking code..." << std::endl;
	std::cout << "=====================" << std::endl << std::endl;

	nextToken();
	graphIterator = states.begin() + 1;

	std::string StartContext = "-1";

	bool result = StmtList(StartContext);
	if (result && it->first == "eof") {
		CodeIsChecked = true;
		CodeIsCorrect = true;

		std::cout << "*********************" << std::endl;
		std::cout << "   Code is Correct!  " << std::endl;
		std::cout << "*********************" << std::endl << std::endl;

		return true;
	} else {
		CodeIsChecked = true;
		CodeIsCorrect = false;

		std::cout << "##########################" << std::endl;
		std::cout << "Your code contains Errors!" << std::endl;
		std::cout << "##########################" << std::endl << std::endl;
		return false;
	}
}

// Function to print graph of your code
void LL::printOutputGraph() {

	if (_graph.empty()) {
		std::cout << "Name for GraphOutput file is not set" << std::endl;
		return;
	}

	if (!CodeIsChecked) {
		if (!checkIfCodeSyntaxIsCorrect()) return;
	}

	if (!CodeIsCorrect) return;

	_printGraph();

	std::cout << "*********************" << std::endl;
	std::cout << "    Graph Printed!" << std::endl;
	std::cout << "*********************" << std::endl << std::endl;
}

// Function to print Atoms of your code
void LL::printAtoms() {

	if (_atomsInput.empty()) {
		std::cout << "Name for AtomsOutput file is not set" << std::endl;
		return;
	}

	if (!CodeIsChecked) {
		if (!checkIfCodeSyntaxIsCorrect()) return;
	}

	if (!CodeIsCorrect) return;

	if (!_printAtoms()) return;

	std::cout << "*********************" << std::endl;
	std::cout << "    Atoms Printed!" << std::endl;
	std::cout << "*********************" << std::endl << std::endl;
}

// Function to generate ASM i8080 code
void LL::printASMCode() {

	if (_asmOut.empty()) {
		std::cout << "Name for ASMOutput file is not set" << std::endl;
		return;
	}

	if (!CodeIsChecked) {
		if (!checkIfCodeSyntaxIsCorrect()) return;
	}

	if (!CodeIsCorrect) return;

	if (!_printASMCode()) return;

	std::cout << "*********************" << std::endl;
	std::cout << "  ASM code Printed!" << std::endl;
	std::cout << "*********************" << std::endl;
}

