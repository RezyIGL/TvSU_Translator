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

	myStream << "ORG 8000H" << std::endl << std::endl;

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
    myStream << "@ MULT ..." << std::endl;
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
void LL::loadOp(const std::string &atom, int shift = 0) {
	if (atom.starts_with('\'')) {

        std::string oper;

        auto iter = sortedAtomsVector.rbegin();
        while (("'" + std::to_string(iter->cnt) + "'") != atom) {
            shift += 2;
            iter++;
        }

        oper = iter->type.substr(2, iter->type.size()) + "_dev_" + iter->name;

        if (iter->scope == "-1") {
            myStream << "LDA " + oper << std::endl;
        } else {
            myStream << "LXI H, " << shift << std::endl;
            myStream << "DAD SP" << std::endl;
            myStream << "MOV A, M" << std::endl;
        }
	} else {
		myStream << "MVI A, " << atom << std::endl;
	}
}

void LL::saveOp(const std::string &atom, int shift = 0) {

    std::string oper;

    auto iter = sortedAtomsVector.rbegin();
    while (("'" + std::to_string(iter->cnt) + "'") != atom) {
        shift += 2;
        iter++;
    }

    oper = iter->type.substr(2, iter->type.size()) + "_dev_" + iter->name;

    if (iter->scope == "-1") {
        myStream << "STA " + oper << std::endl;
    } else {
        myStream << "LXI H, " << shift << std::endl;
        myStream << "DAD SP" << std::endl;
        myStream << "MOV M, A" << std::endl;
    }
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
	 myStream << "OUT 1" << std::endl << std::endl;
}

void LL::CALL(const Atom &atom) {
    /* (CALL, p ,, r)
     * а) PUSH B                  ; Выделяем место под возвращаемое значение
     * б) PUSH B
     *    ...                     ; n раз - выделяем место под аргументы
     *    PUSH B
     * в) Для i = 1 до n:
     *      - снимаем операнд Op с программного стека (paramStack)
     *      - loadOp(Op^(+2n + 2)) # loadOp(std::to_string(stoi(Op) + 2 * n + 2)) # только для локальных переменных
     * г) CALL p
     * д) POP B
     *    ...                     ; n раз - очищаем стек
     *    POP B
     * е) POP B                   ; снимаем результат
     * ж) MOV A, C
     *    saveOp(r)               ; сохраняем результат в r
     * */

    myStream << "PUSH B" << std::endl;

    int n;
    std::string fnToCall;
    for (const auto &i : AtomicMap["-1"]) {
        if (i.cnt == stoi(atom.first.substr(1, atom.first.size() - 2))) {
            n = stoi(i.length);
            fnToCall = i.name;
            break;
        }
    }

    for (int i = 0; i < n; i++) {
        myStream << "PUSH B" << std::endl;
    }

    myStream << std::endl;

    // TODO: FIX
    for (int i = 0; i < n; i++) {
        std::string Op = paramStack.top();

        loadOp(Op, 2 * i + 2);

        paramStack.pop();

        myStream << std::endl;
    }

    myStream << std::endl << "CALL " << fnToCall << std::endl << std::endl;

    for (int i = 0; i < n; i++) {
        myStream << "POP B" << std::endl;
    }

    myStream << "POP B" << std::endl << std::endl;
    myStream << "MOV A, C" << std::endl << std::endl;
    saveOp(atom.third);
}

void LL::PARAM(const Atom &atom) {
	paramStack.push(atom.third);
}

// TODO: Fix
void LL::RET(const Atom &atom) {

    loadOp(atom.third);

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


    // TODO: Generate res
    int res = m * 2 + 2;

    for (int i = 0; i < m; i++) {
        myStream << "POP B" << std::endl;
    }

    myStream << std::endl << "RET" << std::endl << std::endl;
}