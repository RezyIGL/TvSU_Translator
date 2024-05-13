#include "Translator.h"

// Private function to print ASM i8080 code
bool LL::_printASMCode() {
	if (SemanticError) return false;
	if (!generateAtoms()) return false;

	if (entryPoint == "NoEntry") {

		std::cout << "##########################" << std::endl;
		std::cout << "      No Entry Point!     " << std::endl;
		std::cout << "##########################" << std::endl << std::endl;

		return false;
	}

	myStream.open(_asmOut);

	myStream << "ORG 8000H" << std::endl;

	for (const auto &i: sortedAtomsVector) {
		if (i.kind == "var" && i.scope == "-1") {
			if (i.type == "kwint") {
				myStream << "int_dev_" + i.name << ": db ";
				if (i.init.empty()) myStream << "0" << std::endl;
				else myStream << i.init << "" << std::endl;
			} else {
				myStream << "char_dev_" + i.name << ": db ";
				if (i.init.empty()) myStream << "0" << std::endl;
				else myStream << (int) i.init[0] << std::endl;
			}
		}
	}

	myStream << std::endl << "ORG 0" << std::endl << std::endl;
    myStream << "LXI H, 0" << std::endl;
    myStream << "SPHL" << std::endl;
    myStream << "PUSH B" << std::endl;
    myStream << "CALL main" << std::endl;
    myStream << "END" << std::endl << std::endl;
    myStream << "@ DIV ..." << std::endl;
    myStream << "@ PRINT ..." << std::endl << std::endl;

    std::string prevCont = "-1";

	for (const auto &atom: atoms) {

        if (prevCont != atom.context) {
            std::string name;
            int n = 0;
            int m = 0;

            for (const auto &i : AtomicMap["-1"]) {
                if (i.cnt == stoi(atom.context)) {
                    name = i.name;
                    n = stoi(i.length);
                    break;
                }
            }

            myStream << name << ":" << std::endl << std::endl;

            for (const auto &i : sortedAtomsVector) {
                if (i.scope == atom.context && i.kind == "var") m++;
            }

            for (int i = 0; i < m - n; i++) {
                myStream << "PUSH B" << std::endl;
            }

            myStream << std::endl;

            prevCont = atom.context;
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

		else if (atom.text == "IN") IN(atom);  // done
		else if (atom.text == "OUT") OUT(atom);  // done

        // It is really complicated
		else if (atom.text == "CALL") CALL(atom);  // partially done
		else if (atom.text == "PARAM") PARAM(atom);  // done
		else if (atom.text == "RET") RET(atom);  // partially done
	}

    myStream.close();
	return true;
}

// ASM i8080 helpful functions
void LL::loadOp(const std::string &atom, const std::string &scope, int shift = 0) {

    if (atom.starts_with("'")) {
        myStream << "MVI A, " << atom.substr(1, atom.size() - 2) << std::endl;
    } else {
        auto iterator = sortedAtomsVector.rbegin();
        while (std::to_string(iterator->cnt) != atom) {
            if (iterator->scope == scope) {
                if (iterator->kind == "func") shift += 4;
                else shift += 2;
            }
            iterator++;
        }

        if (iterator->scope == "-1") {
            std::string operand = iterator->type.substr(2, iterator->type.size()) + "_dev_" + iterator->name;
            myStream << "LDA " << operand << std::endl;
        } else {
            myStream << "LXI H, " << shift << std::endl;
            myStream << "DAD SP" << std::endl;
            myStream << "MOV A, M" << std::endl;
        }
    }
}

void LL::saveOp(const std::string &atom, const std::string &scope ,int shift = 0) {

    bool OK = true;
    if (shift != 0) OK = false;

    std::string operand;

    auto iterator = sortedAtomsVector.rbegin();
    while (std::to_string(iterator->cnt) != atom) {
        if (OK && iterator->scope == scope) shift += 2;
        iterator++;
    }

    operand = iterator->type.substr(2, iterator->type.size()) + "_dev_" + iterator->name;

    if (iterator->scope == "-1") {
        myStream << "STA " << operand << std::endl;
    } else {
        myStream << "LXI H, " << shift << std::endl;
        myStream << "DAD SP" << std::endl;
        myStream << "MOV M, A" << std::endl;
    }
}

// ASM i8080 translation functions
void LL::MOV(const Atom &atom) {
	loadOp(atom.first, atom.context);
	saveOp(atom.third, atom.context);
	myStream << std::endl;
}

void LL::LBL(const Atom &atom) {
	myStream << atom.third + ":" << std::endl << std::endl;
}

void LL::JMP(const Atom &atom) {
	myStream << "JMP " + atom.third << std::endl;
	myStream << std::endl;
}

void LL::ADD(const Atom &atom) {
	loadOp(atom.second, atom.context);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first, atom.context);
	myStream << "ADD B" << std::endl;
	saveOp(atom.third, atom.context);
	myStream << std::endl;
}

void LL::SUB(const Atom &atom) {
	loadOp(atom.second, atom.context);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first, atom.context);
	myStream << "SUB B" << std::endl;
	saveOp(atom.third, atom.context);
	myStream << std::endl;
}

void LL::OR(const Atom &atom) {
	loadOp(atom.second, atom.context);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first, atom.context);
	myStream << "ORA B" << std::endl;
	saveOp(atom.third, atom.context);
	myStream << std::endl;
}

void LL::AND(const Atom &atom) {
	loadOp(atom.second, atom.context);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first, atom.context);
	myStream << "ANA B" << std::endl;
	saveOp(atom.third, atom.context);
	myStream << std::endl;
}

void LL::EQ(const Atom &atom) {
	loadOp(atom.second, atom.context);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first, atom.context);
	myStream << "CMP B" << std::endl;
	myStream << "JZ " + atom.third << std::endl << std::endl;
}

void LL::NE(const Atom &atom) {
	loadOp(atom.second, atom.context);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first, atom.context);
	myStream << "CMP B" << std::endl;
	myStream << "JNZ " + atom.third << std::endl << std::endl;
}

void LL::GT(const Atom &atom) {
	loadOp(atom.second, atom.context);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first, atom.context);
	myStream << "CMP B" << std::endl;
	myStream << "JP " + atom.third << std::endl << std::endl;
}

void LL::LT(const Atom &atom) {
	loadOp(atom.second, atom.context);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first, atom.context);
	myStream << "CMP B" << std::endl;
	myStream << "JM " + atom.third << std::endl << std::endl;
}

void LL::GE(const Atom &atom) {
	loadOp(atom.second, atom.context);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first, atom.context);
	myStream << "CMP B" << std::endl;
	myStream << "JP " + atom.third << std::endl;
	myStream << "JZ " + atom.third << std::endl << std::endl;
}

void LL::LE(const Atom &atom) {
	loadOp(atom.second, atom.context);
	myStream << "MOV B, A" << std::endl;
	loadOp(atom.first, atom.context);
	myStream << "CMP B" << std::endl;
	myStream << "JM " + atom.third << std::endl;
	myStream << "JZ " + atom.third << std::endl << std::endl;
}

void LL::NOT(const Atom &atom) {
	loadOp(atom.first, atom.context);
	myStream << "CMA" << std::endl;
	saveOp(atom.third, atom.context);
	myStream << std::endl;
}

void LL::MUL(const Atom &atom) {
	MUL_LABEL_START = std::to_string(LabelCnt++);
	MUL_LABEL_END = std::to_string(LabelCnt++);

	// Start
	loadOp(atom.second, atom.context);
	myStream << "MOV C, A" << std::endl;

	loadOp(atom.first, atom.context);
	myStream << "MOV B, A" << std::endl;

	loadOp("0", atom.context);

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
	saveOp(atom.third, atom.context);

    myStream << std::endl;
}

void LL::IN(const Atom &atom) {
	myStream << "IN 0" << std::endl;
	saveOp(atom.third, atom.context);
	myStream << std::endl;
}

void LL::OUT(const Atom &atom) {
	loadOp(atom.third, atom.context);
	 myStream << "OUT 1" << std::endl << std::endl;
}

void LL::CALL(const Atom &atom) {
    auto iterator = AtomicMap["-1"].begin();
    while (std::to_string(iterator->cnt) != atom.first) {
        iterator++;
    }

    int n = stoi(iterator->length);

    myStream << "PUSH B" << std::endl;

    for (int i = 0; i < n; i++) {
        myStream << "PUSH B" << std::endl;
    }

    myStream << std::endl;

    for (int i = 0; i < n; i++) {
        std::string Op = paramStack.top();
        paramStack.pop();

        int shift = 2 * i + 2;

        if (!Op.starts_with("'")) {
            auto inner_iterator = sortedAtomsVector.rbegin();
            while (std::to_string(inner_iterator->cnt) != Op) {
                if (inner_iterator->scope == atom.context) {
                    shift += 2;
                }

                inner_iterator++;
            }
        }

        loadOp(Op, atom.context , shift);

        myStream << "LXI H, " << 2 * i << std::endl;
        myStream << "DAD SP" << std::endl;
        myStream << "MOV M, A" << std::endl;

        myStream << std::endl;
    }

    myStream << "CALL " << iterator->name << std::endl << std::endl;

    for (int i = 0; i < n; i++) {
        myStream << "POP B" << std::endl;
    }

    myStream << "POP B" << std::endl << std::endl;
    myStream << "MOV A, C" << std::endl << std::endl;
    saveOp(atom.third, atom.context);

    myStream << std::endl;
}

void LL::PARAM(const Atom &atom) {
	paramStack.push(atom.third);
}

void LL::RET(const Atom &atom) {
    int m;

    auto iterator = AtomicMap["-1"].begin();
    while (std::to_string(iterator->cnt) != atom.context) {
        iterator++;
    }

    int n = stoi(iterator->length);
    int cnt = 0;

    for (const auto &a: AtomicMap[atom.context]) {
        if (a.kind == "var") cnt++;
    }

    m = cnt - n;

    int res = cnt * 2 + 2;

    if (atom.third.starts_with("'")) {
        myStream << "LXI H, " << res << std::endl;
        myStream << "DAD SP" << std::endl;
        myStream << "MVI M, " << atom.third.substr(1, atom.third.size() - 2) << std::endl;
    } else {
        loadOp(atom.third, atom.context);
        myStream << "LXI H, " << res << std::endl;
        myStream << "DAD SP" << std::endl;
        myStream << "MOV M, A" << std::endl;
    }

    myStream << std::endl;

    for (int i = 0; i < m; i++) {
        myStream << "POP B" << std::endl;
    }

    myStream << std::endl << "RET" << std::endl << std::endl;
}