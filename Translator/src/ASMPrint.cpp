#include "Translator.h"

// Private function to print ASM i8080 code
bool LL::_printASMCode() {
	if (!generateAtoms()) return false;

	if (entryPoint == "NoEntry") {

		std::cout << "##########################" << std::endl;
		std::cout << "      No Entry Point!     " << std::endl;
		std::cout << "##########################" << std::endl << std::endl;

		return false;
	}

	myStream.open(_asmOut);

	myStream << "JMP START" << std::endl << std::endl;

	for (const auto &i: sortedAtomsVector) {
		if (i.kind == "var") {
			if (i.type == "kwint") {
				myStream << sortedAtomsVector[stoi(i.scope)].name + "_int_" + i.name << ": db ";
				if (i.init.empty()) myStream << "0" << std::endl;
				else myStream << i.init << "" << std::endl;
			} else {
				myStream << sortedAtomsVector[stoi(i.scope)].name + "_char_" + i.name << ": dw ";
				if (i.init.empty()) myStream << "0" << std::endl;
				else myStream << (int) i.init[0] << std::endl;
			}
		}
	}

	myStream << std::endl;

	myStream << "START:" << std::endl;
	myStream << "JMP main" << std::endl << std::endl;

	for (const auto &atom: atoms) {
		if (stoi(atom.context) != stoi(currentContext)) {
			doWeHaveLBL = false;
			currentContext = atom.context;
		}

		if (!doWeHaveLBL && (stoi(currentContext) != -1)) {
			doWeHaveLBL = true;
			myStream << sortedAtomsVector[stoi(atom.context)].name + ":" << std::endl << std::endl;
		}

		// Atom translation functions
		if (atom.text == "MOV") MOV(atom); // done
		else if (atom.text == "LBL") LBL(atom); // done
		else if (atom.text == "JMP") JMP(atom); // done
		else if (atom.text == "ADD") ADD(atom); // done
		else if (atom.text == "SUB") SUB(atom); // done
		else if (atom.text == "OR") OR(atom); // done
		else if (atom.text == "AND") AND(atom); // done
		else if (atom.text == "EQ") EQ(atom); // done
		else if (atom.text == "NE") NE(atom); // done
		else if (atom.text == "GT") GT(atom); // done
		else if (atom.text == "LT") LT(atom); // done
		else if (atom.text == "GE") GE(atom); // done
		else if (atom.text == "LE") LE(atom); // done
		else if (atom.text == "NOT") NOT(atom); // done
		else if (atom.text == "MUL") MUL(atom); // done

		else if (atom.text == "IN") IN(atom);  // Not implemented on the eliben.org/js8080/
		else if (atom.text == "OUT") OUT(atom);  // Not implemented on the eliben.org/js8080/

			// It is really complicated
		else if (atom.text == "CALL") CALL(atom);  // I'm lazy for it for now
		else if (atom.text == "PARAM") PARAM(atom);  // I'm lazy for it for now
		else if (atom.text == "RET") RET(atom);  // I'm lazy for it for now
	}

	if (!isUsed) {
		myStream << "main:" << std::endl;
	}

	myStream << "HLT" << std::endl;
	myStream.close();

	return true;
}

// ASM i8080 helpful functions
void LL::loadOp(const std::string &atom) {
	std::string oper;
	if (atom.starts_with('\'')) {
		int SecondId = stoi(atom.substr(1, atom.size() - 1));
		oper = sortedAtomsVector[stoi(sortedAtomsVector[SecondId].scope)].name + "_" +
		       sortedAtomsVector[SecondId].type.substr(2, sortedAtomsVector[SecondId].type.size()) +
		       "_" + sortedAtomsVector[SecondId].name;

		myStream << "LDA " + oper << std::endl;
	} else {
		oper = atom;
		myStream << "MVI A, " << oper << std::endl;
	}
}

void LL::saveOp(const std::string &atom) {
	int TempVarId = stoi(atom.substr(1, atom.size() - 1));
	std::string TempVar = sortedAtomsVector[stoi(sortedAtomsVector[TempVarId].scope)].name + "_" +
	                      sortedAtomsVector[TempVarId].type.substr(2, sortedAtomsVector[TempVarId].type.size()) +
	                      "_" + sortedAtomsVector[TempVarId].name;

	myStream << "STA " + TempVar << std::endl;
}

// ASM i8080 translation functions
void LL::MOV(const Atom &atom) {
	loadOp(atom.first);
	saveOp(atom.third);
	myStream << std::endl;
}

void LL::LBL(const Atom &atom) {
	myStream << atom.third + ":" << std::endl;
	myStream << std::endl;
}

void LL::JMP(const Atom &atom) {
	myStream << "JMP " + atom.third << std::endl;
	myStream << std::endl;
}

void LL::ADD(const Atom &atom) {
	loadOp(atom.second);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first);
	myStream << "ADD B" << std::endl;
	saveOp(atom.third);
	myStream << std::endl;
}

void LL::SUB(const Atom &atom) {
	loadOp(atom.second);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first);
	myStream << "SUB B" << std::endl;
	saveOp(atom.third);
	myStream << std::endl;
}

void LL::OR(const Atom &atom) {
	loadOp(atom.second);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first);
	myStream << "ORA B" << std::endl;
	saveOp(atom.third);
	myStream << std::endl;
}

void LL::AND(const Atom &atom) {
	loadOp(atom.second);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first);
	myStream << "ANA B" << std::endl;
	saveOp(atom.third);
	myStream << std::endl;
}

void LL::EQ(const Atom &atom) {
	loadOp(atom.second);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first);
	myStream << "CMP B" << std::endl;
	myStream << "JZ " + atom.third << std::endl << std::endl;
}

void LL::NE(const Atom &atom) {
	loadOp(atom.second);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first);
	myStream << "CMP B" << std::endl;
	myStream << "JNZ " + atom.third << std::endl << std::endl;
}

void LL::GT(const Atom &atom) {
	loadOp(atom.second);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first);
	myStream << "CMP B" << std::endl;
	myStream << "JP " + atom.third << std::endl << std::endl;
}

void LL::LT(const Atom &atom) {
	loadOp(atom.second);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first);
	myStream << "CMP B" << std::endl;
	myStream << "JM " + atom.third << std::endl << std::endl;
}

void LL::GE(const Atom &atom) {
	loadOp(atom.second);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first);
	myStream << "CMP B" << std::endl;
	myStream << "JP " + atom.third << std::endl;
	myStream << "JZ " + atom.third << std::endl << std::endl;
}

void LL::LE(const Atom &atom) {
	loadOp(atom.second);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first);
	myStream << "CMP B" << std::endl;
	myStream << "JM " + atom.third << std::endl;
	myStream << "JZ " + atom.third << std::endl << std::endl;
}

void LL::NOT(const Atom &atom) {
	loadOp(atom.first);
	myStream << "CMA" << std::endl;
	saveOp(atom.third);
	myStream << std::endl;
}

void LL::MUL(const Atom &atom) {
	MUL_LABEL_START = std::to_string(LabelCnt++);
	MUL_LABEL_END = std::to_string(LabelCnt++);

	// Start
	loadOp(atom.second);
	myStream << "MOV C, A" << std::endl;

	loadOp(atom.first);
	myStream << "MOV B, A" << std::endl;

	loadOp("0");

	myStream << "CMP C" << std::endl;
	myStream << "JZ L" + MUL_LABEL_END << std::endl;

	myStream << "JMP L" + MUL_LABEL_START << std::endl;
	myStream << std::endl;

	// Multiplication
	myStream << "L" + MUL_LABEL_START + ":" << std::endl;
	myStream << "ADD B" << std::endl;

	myStream << "DCR C" << std::endl;
	myStream << "JZ L" + MUL_LABEL_END << std::endl;

	myStream << "JMP L" + MUL_LABEL_START << std::endl << std::endl;

	// End
	myStream << "L" + MUL_LABEL_END + ":" << std::endl;
	saveOp(atom.third);
}

void LL::IN(const Atom &atom) {
	myStream << "IN 0" << std::endl;
	saveOp(atom.third);
	myStream << std::endl;
}

void LL::OUT(const Atom &atom) {
	loadOp(atom.third);
	// myStream << "OUT 1" << std::endl << std::endl;
}

// useless (as they're now) functions

void LL::CALL(const Atom &atom) {
	myStream << "; CALL IS NOT IMPLEMENTED" << std::endl << std::endl;
}

void LL::PARAM(const Atom &atom) {
	myStream << "; PARAM IS NOT IMPLEMENTED" << std::endl << std::endl;
}

void LL::RET(const Atom &atom) {
	myStream << "; RET IS NOT IMPLEMENTED" << std::endl << std::endl;
}