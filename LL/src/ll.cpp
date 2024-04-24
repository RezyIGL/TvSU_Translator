#include "ll.h"
#include <iostream>
#include <algorithm>

LL::LL(std::istream &stream, const std::string &graphFilePath, const std::string &atomOutputPath,
       const std::string &asmOutputPath)
		: lexer{stream} {
	_graph = graphFilePath;
	_atomsInput = atomOutputPath;
	_asmOut = asmOutputPath;
}

// Functions to print all needed files.
void LL::printGraph() {
	myStream.open(_graph);

	while (!outputVector.empty()) {
		myStream << outputVector.front() << std::endl;
		outputVector.erase(outputVector.begin());
	}

	myStream.close();
}

bool LL::printAtoms() {
	myStream.open(_atomsInput);
	myStream << "Context: Atom\n" << std::endl;

	if (!atoms.empty() && atoms.front().text == "ERROR") {
		std::cout << "\n===========[Your code contains Semantic Errors!]===========" << std::endl;
		myStream.close();
		return false;
	}

	for (const auto &i: atoms) {
		myStream << i.context << ": ("
		         << i.text << ","
		         << i.first << ","
		         << i.second << ","
		         << i.third << ")" << std::endl;
	}

	myStream << "\n=============================================\nName : Code : Class : Type : Init : Scope \n\n";

	for (const auto &i: AtomicMap) {
		for (const auto &j: i.second) {
			sortedAtomsVector.emplace_back(j);
		}
	}

	struct {
		bool operator()(VarOrFunc &a, VarOrFunc &b) const { return a.cnt < b.cnt; };
	} customLess;
	std::sort(sortedAtomsVector.begin(), sortedAtomsVector.end(), customLess);

	for (const auto &i: sortedAtomsVector) {
		myStream << i.name << " : '" << i.cnt << "' : " << i.kind << " : " << i.type << " : " << i.init
		         << " : " << i.scope << std::endl;
	}

	myStream.close();
	return true;
}

void LL::printASMCode() {
	myStream.open(_asmOut);

	myStream << "JMP START" << std::endl << std::endl;

	for (const auto &i: sortedAtomsVector) {
		if (i.kind == "var") {
			if (i.type == "kwint") {
				myStream << "int_" + i.name << ": db ";
				if (i.init.empty()) myStream << "0" << std::endl;
				else myStream << i.init << "" << std::endl;
			} else {
				myStream << "char_" + i.name << ": dw ";
				if (i.init.empty()) myStream << "\"\"" << std::endl;
				else myStream << "\"" << i.init << "\"" << std::endl;
			}
		}
	}

	myStream << std::endl;

	myStream << "START:" << std::endl << std::endl;

	for (const auto &atom: atoms) {
		if (atom.text == "MOV") MOV(atom); // done
		if (atom.text == "LBL") LBL(atom); // done
		if (atom.text == "JMP") JMP(atom); // done
		if (atom.text == "ADD") ADD(atom); // done
		if (atom.text == "SUB") SUB(atom); // done
		if (atom.text == "OR") OR(atom); // done
		if (atom.text == "AND") AND(atom); // done
		if (atom.text == "EQ") EQ(atom); // done
		if (atom.text == "NE") NE(atom); // done
		if (atom.text == "GT") GT(atom); // done
		if (atom.text == "LT") LT(atom); // done
		if (atom.text == "GE") GE(atom); // done
		if (atom.text == "LE") LE(atom); // done
		if (atom.text == "NOT") NOT(atom); // done

		// It is really complicated or eliben.org/js8080/ didn't implement it
		if (atom.text == "MUL") MUL(atom); // cannot
		if (atom.text == "CALL") CALL(atom);  // cannot
		if (atom.text == "PARAM") PARAM(atom);  // cannot
		if (atom.text == "RET") RET(atom);  // cannot
		if (atom.text == "IN") IN(atom);  // cannot
		if (atom.text == "OUT") OUT(atom);  // cannot
	}

	myStream.close();
}

// Main logic functions
void LL::isTranslated() {
	printGraph();
	std::cout << "\n==========[Graph printed.]=============" << std::endl;

	if (!printAtoms()) return;
	std::cout << "\n==========[Atoms printed.]=============" << std::endl;

	printASMCode();
	std::cout << "\n==========[ASM code printed.]==========" << std::endl;

	std::cout << "\n==========[All done!]==================" << std::endl;
}

void LL::validate() {
	nextToken();
	graphIterator = states.begin() + 1;

	generateString("StmtList");
	std::string StartContext = "-1";

	bool result = StmtList(StartContext);

	if (result && it->first == "eof") isTranslated();
	else std::cout << "==========[Your code contains error!]==========" << std::endl;
}

void LL::generateString(const std::string &TextToAdd) {
	std::string _text;

	for (auto i = states.begin(); i != states.end(); ++i) {
		if (i == states.end() - 1) {
			if (*i == 1) {
				_text += "├";
			} else {
				_text += "└";
			}

			break;
		}

		if (*i == 1) {
			_text += "│ ";
		} else {
			_text += " ";
		}
	}

	outVecCnt++;
	outputVector.push_back(_text + TextToAdd);
}

// ASM i8080 translation functions
void LL::MOV(const Atom &atom) {
	if (atom.first.starts_with('\'')) {
		int idFirst = stoi(atom.first.substr(1, atom.first.size() - 1));
		std::string lh = sortedAtomsVector[idFirst].type.substr(2, sortedAtomsVector[idFirst].type.size()) +
				"_" + sortedAtomsVector[idFirst].name;

		int idSecond = stoi(atom.third.substr(1, atom.third.size() - 1));
		std::string rh = sortedAtomsVector[idSecond].type.substr(2, sortedAtomsVector[idSecond].type.size()) +
				"_" + sortedAtomsVector[idSecond].name;

		myStream << "LDA " + lh << std::endl;
		myStream << "STA " + rh << std::endl;
	} else {
		int idSecond = stoi(atom.third.substr(1, atom.third.size() - 1));
		std::string rh = sortedAtomsVector[idSecond].type.substr(2, sortedAtomsVector[idSecond].type.size()) +
				"_" + sortedAtomsVector[idSecond].name;

		myStream << "MVI A, " + atom.first << std::endl;
		myStream << "STA " + rh << std::endl;
	}

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

	std::string LeftHand;
	std::string RightHand;

	if (atom.second.starts_with('\'')) {
		int SecondId = stoi(atom.second.substr(1, atom.second.size() - 1));
		RightHand = sortedAtomsVector[SecondId].type.substr(2, sortedAtomsVector[SecondId].type.size()) +
		            "_" + sortedAtomsVector[SecondId].name;

		myStream << "LDA " + RightHand << std::endl;
	} else {
		RightHand = atom.second;
		myStream << "MVI A, " << RightHand << std::endl;
	}

	myStream << "MOV B, A" << std::endl;

	if (atom.first.starts_with('\'')) {
		int FirstId = stoi(atom.first.substr(1, atom.first.size() - 1));
		LeftHand = sortedAtomsVector[FirstId].type.substr(2, sortedAtomsVector[FirstId].type.size()) +
		           "_" + sortedAtomsVector[FirstId].name;

		myStream << "LDA " + LeftHand << std::endl;
	} else {
		LeftHand = atom.first;
		myStream << "MVI A, " << LeftHand << std::endl;
	}

	int TempVarId = stoi(atom.third.substr(1, atom.third.size() - 1));
	std::string TempVar = sortedAtomsVector[TempVarId].type.substr(2, sortedAtomsVector[TempVarId].type.size()) +
	                 "_" + sortedAtomsVector[TempVarId].name;

	myStream << "ADD B" << std::endl;
	myStream << "STA " + TempVar << std::endl;

	myStream << std::endl;
}

void LL::SUB(const Atom &atom) {
	std::string LeftHand;
	std::string RightHand;

	if (atom.second.starts_with('\'')) {
		int SecondId = stoi(atom.second.substr(1, atom.second.size() - 1));
		RightHand = sortedAtomsVector[SecondId].type.substr(2, sortedAtomsVector[SecondId].type.size()) +
		            "_" + sortedAtomsVector[SecondId].name;

		myStream << "LDA " + RightHand << std::endl;
	} else {
		RightHand = atom.second;
		myStream << "MVI A, " << RightHand << std::endl;
	}

	myStream << "MOV B, A" << std::endl;

	if (atom.first.starts_with('\'')) {
		int FirstId = stoi(atom.first.substr(1, atom.first.size() - 1));
		LeftHand = sortedAtomsVector[FirstId].type.substr(2, sortedAtomsVector[FirstId].type.size()) +
		           "_" + sortedAtomsVector[FirstId].name;

		myStream << "LDA " + LeftHand << std::endl;
	} else {
		LeftHand = atom.first;
		myStream << "MVI A, " << LeftHand << std::endl;
	}

	int TempVarId = stoi(atom.third.substr(1, atom.third.size() - 1));
	std::string TempVar = sortedAtomsVector[TempVarId].type.substr(2, sortedAtomsVector[TempVarId].type.size()) +
	                      "_" + sortedAtomsVector[TempVarId].name;

	myStream << "SUB B" << std::endl;
	myStream << "STA " + TempVar << std::endl;

	myStream << std::endl;
}

void LL::OR(const Atom &atom) {
	std::string LeftHand;
	std::string RightHand;

	if (atom.second.starts_with('\'')) {
		int SecondId = stoi(atom.second.substr(1, atom.second.size() - 1));
		RightHand = sortedAtomsVector[SecondId].type.substr(2, sortedAtomsVector[SecondId].type.size()) +
		            "_" + sortedAtomsVector[SecondId].name;

		myStream << "LDA " + RightHand << std::endl;
	} else {
		RightHand = atom.second;
		myStream << "MVI A, " << RightHand << std::endl;
	}

	myStream << "MOV B, A" << std::endl;

	if (atom.first.starts_with('\'')) {
		int FirstId = stoi(atom.first.substr(1, atom.first.size() - 1));
		LeftHand = sortedAtomsVector[FirstId].type.substr(2, sortedAtomsVector[FirstId].type.size()) +
		           "_" + sortedAtomsVector[FirstId].name;

		myStream << "LDA " + LeftHand << std::endl;
	} else {
		LeftHand = atom.first;
		myStream << "MVI A, " << LeftHand << std::endl;
	}

	int TempVarId = stoi(atom.third.substr(1, atom.third.size() - 1));
	std::string TempVar = sortedAtomsVector[TempVarId].type.substr(2, sortedAtomsVector[TempVarId].type.size()) +
	                      "_" + sortedAtomsVector[TempVarId].name;

	myStream << "ORA B" << std::endl;
	myStream << "STA " + TempVar << std::endl;

	myStream << std::endl;
}

void LL::AND(const Atom &atom) {
	std::string LeftHand;
	std::string RightHand;

	if (atom.second.starts_with('\'')) {
		int SecondId = stoi(atom.second.substr(1, atom.second.size() - 1));
		RightHand = sortedAtomsVector[SecondId].type.substr(2, sortedAtomsVector[SecondId].type.size()) +
		            "_" + sortedAtomsVector[SecondId].name;

		myStream << "LDA " + RightHand << std::endl;
	} else {
		RightHand = atom.second;
		myStream << "MVI A, " << RightHand << std::endl;
	}

	myStream << "MOV B, A" << std::endl;

	if (atom.first.starts_with('\'')) {
		int FirstId = stoi(atom.first.substr(1, atom.first.size() - 1));
		LeftHand = sortedAtomsVector[FirstId].type.substr(2, sortedAtomsVector[FirstId].type.size()) +
		           "_" + sortedAtomsVector[FirstId].name;

		myStream << "LDA " + LeftHand << std::endl;
	} else {
		LeftHand = atom.first;
		myStream << "MVI A, " << LeftHand << std::endl;
	}

	int TempVarId = stoi(atom.third.substr(1, atom.third.size() - 1));
	std::string TempVar = sortedAtomsVector[TempVarId].type.substr(2, sortedAtomsVector[TempVarId].type.size()) +
	                      "_" + sortedAtomsVector[TempVarId].name;

	myStream << "ANA B" << std::endl;
	myStream << "STA " + TempVar << std::endl;

	myStream << std::endl;
}

void LL::EQ(const Atom &atom) {
	std::string LeftHand;
	std::string RightHand;

	if (atom.second.starts_with('\'')) {
		int SecondId = stoi(atom.second.substr(1, atom.second.size() - 1));
		RightHand = sortedAtomsVector[SecondId].type.substr(2, sortedAtomsVector[SecondId].type.size()) +
		            "_" + sortedAtomsVector[SecondId].name;

		myStream << "LDA " + RightHand << std::endl;
	} else {
		RightHand = atom.second;
		myStream << "MVI A, " << RightHand << std::endl;
	}

	myStream << "MOV B, A" << std::endl;

	if (atom.first.starts_with('\'')) {
		int FirstId = stoi(atom.first.substr(1, atom.first.size() - 1));
		LeftHand = sortedAtomsVector[FirstId].type.substr(2, sortedAtomsVector[FirstId].type.size()) +
		           "_" + sortedAtomsVector[FirstId].name;

		myStream << "LDA " + LeftHand << std::endl;
	} else {
		LeftHand = atom.first;
		myStream << "MVI A, " << LeftHand << std::endl;
	}

	myStream << "SUB B" << std::endl;
	myStream << "JZ " + atom.third << std::endl << std::endl;
}

void LL::NE(const Atom &atom) {
	std::string LeftHand;
	std::string RightHand;

	if (atom.second.starts_with('\'')) {
		int SecondId = stoi(atom.second.substr(1, atom.second.size() - 1));
		RightHand = sortedAtomsVector[SecondId].type.substr(2, sortedAtomsVector[SecondId].type.size()) +
		            "_" + sortedAtomsVector[SecondId].name;

		myStream << "LDA " + RightHand << std::endl;
	} else {
		RightHand = atom.second;
		myStream << "MVI A, " << RightHand << std::endl;
	}

	myStream << "MOV B, A" << std::endl;

	if (atom.first.starts_with('\'')) {
		int FirstId = stoi(atom.first.substr(1, atom.first.size() - 1));
		LeftHand = sortedAtomsVector[FirstId].type.substr(2, sortedAtomsVector[FirstId].type.size()) +
		           "_" + sortedAtomsVector[FirstId].name;

		myStream << "LDA " + LeftHand << std::endl;
	} else {
		LeftHand = atom.first;
		myStream << "MVI A, " << LeftHand << std::endl;
	}

	myStream << "SUB B" << std::endl;
	myStream << "JNZ " + atom.third << std::endl << std::endl;
}

void LL::GT(const Atom &atom) {
	std::string LeftHand;
	std::string RightHand;

	if (atom.second.starts_with('\'')) {
		int SecondId = stoi(atom.second.substr(1, atom.second.size() - 1));
		RightHand = sortedAtomsVector[SecondId].type.substr(2, sortedAtomsVector[SecondId].type.size()) +
		            "_" + sortedAtomsVector[SecondId].name;

		myStream << "LDA " + RightHand << std::endl;
	} else {
		RightHand = atom.second;
		myStream << "MVI A, " << RightHand << std::endl;
	}

	myStream << "MOV B, A" << std::endl;

	if (atom.first.starts_with('\'')) {
		int FirstId = stoi(atom.first.substr(1, atom.first.size() - 1));
		LeftHand = sortedAtomsVector[FirstId].type.substr(2, sortedAtomsVector[FirstId].type.size()) +
		           "_" + sortedAtomsVector[FirstId].name;

		myStream << "LDA " + LeftHand << std::endl;
	} else {
		LeftHand = atom.first;
		myStream << "MVI A, " << LeftHand << std::endl;
	}

	myStream << "SUB B" << std::endl;
	myStream << "JP " + atom.third << std::endl << std::endl;
}

void LL::LT(const Atom &atom) {
	std::string LeftHand;
	std::string RightHand;

	if (atom.second.starts_with('\'')) {
		int SecondId = stoi(atom.second.substr(1, atom.second.size() - 1));
		RightHand = sortedAtomsVector[SecondId].type.substr(2, sortedAtomsVector[SecondId].type.size()) +
		            "_" + sortedAtomsVector[SecondId].name;

		myStream << "LDA " + RightHand << std::endl;
	} else {
		RightHand = atom.second;
		myStream << "MVI A, " << RightHand << std::endl;
	}

	myStream << "MOV B, A" << std::endl;

	if (atom.first.starts_with('\'')) {
		int FirstId = stoi(atom.first.substr(1, atom.first.size() - 1));
		LeftHand = sortedAtomsVector[FirstId].type.substr(2, sortedAtomsVector[FirstId].type.size()) +
		           "_" + sortedAtomsVector[FirstId].name;

		myStream << "LDA " + LeftHand << std::endl;
	} else {
		LeftHand = atom.first;
		myStream << "MVI A, " << LeftHand << std::endl;
	}

	myStream << "SUB B" << std::endl;
	myStream << "JM " + atom.third << std::endl << std::endl;
}

void LL::GE(const Atom &atom) {
	std::string LeftHand;
	std::string RightHand;

	if (atom.second.starts_with('\'')) {
		int SecondId = stoi(atom.second.substr(1, atom.second.size() - 1));
		RightHand = sortedAtomsVector[SecondId].type.substr(2, sortedAtomsVector[SecondId].type.size()) +
		            "_" + sortedAtomsVector[SecondId].name;

		myStream << "LDA " + RightHand << std::endl;
	} else {
		RightHand = atom.second;
		myStream << "MVI A, " << RightHand << std::endl;
	}

	myStream << "MOV B, A" << std::endl;

	if (atom.first.starts_with('\'')) {
		int FirstId = stoi(atom.first.substr(1, atom.first.size() - 1));
		LeftHand = sortedAtomsVector[FirstId].type.substr(2, sortedAtomsVector[FirstId].type.size()) +
		           "_" + sortedAtomsVector[FirstId].name;

		myStream << "LDA " + LeftHand << std::endl;
	} else {
		LeftHand = atom.first;
		myStream << "MVI A, " << LeftHand << std::endl;
	}

	myStream << "SUB B" << std::endl;
	myStream << "JP " + atom.third << std::endl;
	myStream << "JZ " + atom.third << std::endl << std::endl;
}

void LL::LE(const Atom &atom) {
	std::string LeftHand;
	std::string RightHand;

	if (atom.second.starts_with('\'')) {
		int SecondId = stoi(atom.second.substr(1, atom.second.size() - 1));
		RightHand = sortedAtomsVector[SecondId].type.substr(2, sortedAtomsVector[SecondId].type.size()) +
		            "_" + sortedAtomsVector[SecondId].name;

		myStream << "LDA " + RightHand << std::endl;
	} else {
		RightHand = atom.second;
		myStream << "MVI A, " << RightHand << std::endl;
	}

	myStream << "MOV B, A" << std::endl;

	if (atom.first.starts_with('\'')) {
		int FirstId = stoi(atom.first.substr(1, atom.first.size() - 1));
		LeftHand = sortedAtomsVector[FirstId].type.substr(2, sortedAtomsVector[FirstId].type.size()) +
		           "_" + sortedAtomsVector[FirstId].name;

		myStream << "LDA " + LeftHand << std::endl;
	} else {
		LeftHand = atom.first;
		myStream << "MVI A, " << LeftHand << std::endl;
	}

	myStream << "SUB B" << std::endl;
	myStream << "JM " + atom.third << std::endl;
	myStream << "JZ " + atom.third << std::endl << std::endl;
}

void LL::NOT(const Atom &atom) {
	std::string LeftHand;

	if (atom.first.starts_with('\'')) {
		int FirstId = stoi(atom.first.substr(1, atom.first.size() - 1));
		LeftHand = sortedAtomsVector[FirstId].type.substr(2, sortedAtomsVector[FirstId].type.size()) +
		           "_" + sortedAtomsVector[FirstId].name;

		myStream << "LDA " + LeftHand << std::endl;
	} else {
		LeftHand = atom.first;
		myStream << "MVI A, " << LeftHand << std::endl;
	}

	myStream << "CMA" << std::endl;

	int TempVarId = stoi(atom.third.substr(1, atom.third.size() - 1));
	std::string TempVar = sortedAtomsVector[TempVarId].type.substr(2, sortedAtomsVector[TempVarId].type.size()) +
	                      "_" + sortedAtomsVector[TempVarId].name;

	myStream << "STA " + TempVar << std::endl;

	myStream << std::endl;
}

// useless (as they're now) functions

void LL::MUL(const Atom &atom) {
	myStream << "; MUL IS NOT IMPLEMENTED" << std::endl << std::endl;
}

void LL::CALL(const Atom &atom) {
	myStream << "; CALL IS NOT IMPLEMENTED" << std::endl << std::endl;
}

void LL::PARAM(const Atom &atom) {
	myStream << "; PARAM IS NOT IMPLEMENTED" << std::endl << std::endl;
}

void LL::RET(const Atom &atom) {
	myStream << "; RET IS NOT IMPLEMENTED" << std::endl << std::endl;
}

void LL::IN(const Atom &atom) {
	myStream << "; IN IS NOT IMPLEMENTED" << std::endl << std::endl;
}

void LL::OUT(const Atom &atom) {
	myStream << "; OUT IS NOT IMPLEMENTED" << std::endl << std::endl;
}

//	// LL analyzer check functions

// Semantic analyzer functions
std::string LL::newLabel() {
	return std::to_string(LabelCnt++);
}

std::string LL::alloc(const std::string &scope) {
	std::string temp = addVar("T" + std::to_string(NewVarCnt++), scope, "kwint", "0");
	return "'" + temp + "'";
}

std::string
LL::addVar(const std::string &name, const std::string &scope, const std::string &type, const std::string &init = "") {
	if (AtomicMap.count(scope)) {
		for (const auto &i: AtomicMap[scope]) {
			if (i.name == name) {
				return "ERROR";
			}
		}
	}

	VarOrFunc temp = {name, scope, type, init, "var", AtomicMapCnt++};
	AtomicMap[scope].emplace_back(temp);

	return std::to_string(temp.cnt);
}

std::string LL::addFunc(const std::string &name, const std::string &type, const std::string &length = "0") {
	for (const auto &i: AtomicMap["-1"]) {
		if (i.name == name) {
			return "ERROR";
		}
	}

	VarOrFunc temp = {name, "-1", type, length.empty() ? "0" : length, "func", AtomicMapCnt++};
	AtomicMap["-1"].emplace_back(temp);

	return std::to_string(temp.cnt);
}

std::string LL::checkVar(const std::string &scope, const std::string &name) {
	for (const auto &i: AtomicMap[scope]) {
		if (i.name == name && i.kind == "var") return "'" + std::to_string(i.cnt) + "'";
		else if (i.name == name && i.type != "var") return "ERROR";
	}

	for (const auto &i: AtomicMap["-1"]) {
		if (i.name == name && i.kind == "var") return "'" + std::to_string(i.cnt) + "'";
		else if (i.name == name && i.type != "var") return "ERROR";
	}

	return "ERROR";
}

std::string LL::checkFunc(const std::string &name, const std::string &len) {
	for (const auto &i: AtomicMap["-1"]) {
		if (i.name == name && i.kind == "func" && i.init == len) return "'" + std::to_string(i.cnt) + "'";
		else if (i.name == name && i.type != "func") return "ERROR";
	}

	return "ERROR";
}

void LL::generateAtom(const std::string &context = "", const std::string &text = "", const std::string &first = "",
                      const std::string &second = "", const std::string &third = "") {
	if (atoms.size() == 1 && atoms[0].text == "ERROR") return;

	if (context == "ERROR" || text == "ERROR" || first == "ERROR" || second == "ERROR" || third == "ERROR") {
		atoms.clear();
		atoms.emplace_back("SYSTEM", "ERROR", "ERROR", "ERROR", "ERROR");
		return;
	}

	Atom atom = {context, text, first, second, third};
	atoms.emplace_back(atom);
}

// Syntax analyzer and graph creating functions
void LL::nextToken() {
	if (lexemVector.empty() || it == lexemVector.end() - 1) {
		lexemVector.emplace_back(lexer.getNextToken());
		it = lexemVector.end() - 1;
		return;
	}

	it++;
}

// Generates next state (0 || 1) to make correct calculations
void LL::nextGraphState(const int &state) {
	states.emplace_back(state);
	graphIterator = states.end() - 1;
}

// Literally deletes last state and rolls iterator back
void LL::rollbackGraphNode() {
	graphIterator = (graphIterator > states.begin()) ? graphIterator - 1 : states.begin();
	states.pop_back();
}

// MiniC grammar check
bool LL::StmtList(const std::string &context) {
	if (it->first == "eof") return true;

	auto _temp = it;

	nextGraphState(1);
	generateString("Stmt");

	bool result = Stmt(context);
	if (!result) {
		outputVector.pop_back();

		it = _temp;

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	nextGraphState(0);
	generateString("StmtList");

	bool tailResult = StmtList(context);
	if (!tailResult) return false;

	rollbackGraphNode();
	return true;
}

bool LL::Stmt(const std::string &context) {
	if (it->first == "eof") return false;

	if (it->first == "kwint" || it->first == "kwchar") {
		nextGraphState(0);
		generateString("DeclareStmt");

		if (!DeclareStmt(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "semicolon") {
		nextToken();

		nextGraphState(0);
		generateString("semicolon");

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	if (context == "-1") return false;

	if (it->first == "id") {
		nextGraphState(0);
		generateString("AssignOrCallOp");

		if (!AssignOrCallOp(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwwhile") {
		nextToken();

		nextGraphState(0);
		generateString("kwwhile WhileOp");

		if (!WhileOp(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwfor") {
		nextToken();

		nextGraphState(0);
		generateString("kwfor ForOp");

		if (!ForOp(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwif") {
		nextToken();

		nextGraphState(0);
		generateString("kwif IfOp");

		if (!IfOp(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwswitch") {
		nextToken();

		nextGraphState(0);
		generateString("kwswitch SwitchOp");

		if (!SwitchOp(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwin") {
		nextToken();

		nextGraphState(0);
		generateString("kwin InOp");

		if (!InOp(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwout") {
		nextToken();

		nextGraphState(0);
		generateString("kwout OutOp");

		if (!OutOp(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "lbrace") {
		nextToken();

		nextGraphState(1);
		generateString("lbrace StmtList");

		bool result = StmtList(context);
		if (!result) return false;

		if (it->first != "rbrace") return false;
		nextToken();

		nextGraphState(0);
		generateString("rbrace");

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwreturn") {
		nextToken();

		nextGraphState(1);
		generateString("kwreturn E");

		auto EResult = Expr(context);
		if (!EResult.first) return false;

		generateAtom(context, "RET", "", "", EResult.second);


		if (it->first != "semicolon") return false;
		nextToken();

		nextGraphState(0);
		generateString("semicolon");

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	return false;
}

bool LL::DeclareStmt(const std::string &context) {

	nextGraphState(1);
	generateString("Type");

	auto TypeResult = Type(context);
	if (!TypeResult.first) return false;

	if (it->first != "id") return false;
	auto temp = it->second;
	nextToken();

	nextGraphState(0);
	generateString(temp + " DeclareStmt'");

	if (!DeclareStmtList(context, TypeResult.second, temp)) return false;

	rollbackGraphNode();
	return true;
}

bool LL::DeclareStmtList(const std::string &context, const std::string &type, const std::string &name) {
	if (it->first == "eof") return false;

	if (it->first == "lpar") {
		if (stoi(context) > -1) return false;
		nextToken();

		nextGraphState(1);
		generateString("lpar ParamList");

		auto ParamListRes = ParamList(context);
		if (!ParamListRes.first) return false;

		if (it->first != "rpar") return false;
		nextToken();

		auto TC = addFunc(name, type, ParamListRes.second);

		if (it->first != "lbrace") return false;
		nextToken();

		nextGraphState(1);
		generateString("rpar lbrace StmtList");

		if (!StmtList(TC)) return false;

		if (it->first != "rbrace") return false;
		nextToken();

		generateAtom(context, "RET", "", "", "0");

		nextGraphState(0);
		generateString("rbrace");

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	} else if (it->first == "opassign") {
		nextToken();
		auto _temp = it->second;

		if (it->first == "num") {
			addVar(name, context, type, _temp);
			nextToken();

			nextGraphState(1);
			generateString("opassign " + _temp + " DeclareVarList");

			auto DecVarListRes = DeclareVarList(context, type);
			if (!DecVarListRes) return false;

			if (it->first != "semicolon") return false;
			nextToken();

			nextGraphState(0);
			generateString("semicolon");

			rollbackGraphNode();
			rollbackGraphNode();
			return true;
		} else if (it->first == "char") {
			addVar(name, context, type, _temp);
			nextToken();

			nextGraphState(1);
			generateString("opassign " + _temp + "DeclareVarList");

			auto DecVarListRes = DeclareVarList(context, type);
			if (!DecVarListRes) return false;

			if (it->first != "semicolon") return false;
			nextToken();

			nextGraphState(0);
			generateString("semicolon");

			rollbackGraphNode();
			rollbackGraphNode();
			return true;
		} else {
			return false;
		}
	} else {
		addVar(name, context, type);

		nextGraphState(1);
		generateString("DeclareVarList");

		if (!DeclareVarList(context, type)) return false;

		if (it->first != "semicolon") return false;
		nextToken();

		nextGraphState(0);
		generateString("semicolon");

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}
}

bool LL::AssignOrCallOp(const std::string &context) {

	nextGraphState(1);
	generateString("AssignOrCall");

	if (!AssignOrCall(context)) return false;

	if (it->first != "semicolon") return false;
	nextToken();

	nextGraphState(0);
	generateString("semicolon");

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::AssignOrCall(const std::string &context) {
	if (it->first != "id") return false;
	auto temp = it->second;
	nextToken();

	nextGraphState(0);
	generateString(temp + " AssignOrCall'");

	if (!AssignOrCallList(context, temp)) return false;

	rollbackGraphNode();
	return true;
}

bool LL::AssignOrCallList(const std::string &context, const std::string &name) {
	if (it->first == "opassign") {
		nextToken();

		nextGraphState(0);
		generateString("opassign E");

		auto ERes = Expr(context);
		if (!ERes.first) return false;

		auto r = checkVar(context, name);
		generateAtom(context, "MOV", ERes.second, "", r);

		rollbackGraphNode();
		return true;
	}

	if (it->first == "lpar") {
		nextToken();

		nextGraphState(1);
		generateString("lpar ArgList");

		auto ArgListRes = ArgList(context);
		if (!ArgListRes.first) return false;

		if (it->first != "rpar") return false;
		nextToken();

		nextGraphState(0);
		generateString("rpar");

		auto q = checkFunc(name, ArgListRes.second);
		auto r = alloc(context);
		generateAtom(context, "CALL", q, "", r);

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	return false;
}

bool LL::WhileOp(const std::string &context) {

	auto l1 = newLabel();
	auto l2 = newLabel();

	generateAtom(context, "LBL", "", "", "L" + l1);

	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("lpar E");

	auto ERes = Expr(context);
	if (!ERes.first) return false;

	generateAtom(context, "EQ", ERes.second, "0", "L" + l2);

	if (it->first != "rpar") return false;
	nextToken();

	nextGraphState(0);
	generateString("rpar Stmt");

	if (!Stmt(context)) return false;

	generateAtom(context, "JMP", "", "", "L" + l1);
	generateAtom(context, "LBL", "", "", "L" + l2);

	rollbackGraphNode();
	return true;
}

bool LL::ForOp(const std::string &context) {

	auto l1 = newLabel();
	auto l2 = newLabel();
	auto l3 = newLabel();
	auto l4 = newLabel();

	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("lpar ForInit");

	if (!ForInit(context)) return false;

	generateAtom(context, "LBL", "", "", "L" + l1);

	nextGraphState(1);
	generateString("semicolon ForExp");

	FT ForExpRes = ForExp(context);
	if (!ForExpRes.first) return false;

	generateAtom(context, "EQ", ForExpRes.second, "0", "L" + l4);
	generateAtom(context, "JMP", "", "", "L" + l3);
	generateAtom(context, "LBL", "", "", "L" + l2);

	nextGraphState(1);
	generateString("semicolon ForLoop");

	if (!ForLoop(context)) return false;

	generateAtom(context, "JMP", "", "", "L" + l1);
	generateAtom(context, "LBL", "", "", "L" + l3);

	nextGraphState(0);
	generateString("rpar Stmt");

	if (!Stmt(context)) return false;

	generateAtom(context, "JMP", "", "", "L" + l2);
	generateAtom(context, "LBL", "", "", "L" + l4);

	rollbackGraphNode();
	return true;
}

bool LL::ForInit(const std::string &context) {
	if (it->first == "id") {
		nextGraphState(0);
		generateString("AssignOrCall");

		if (!AssignOrCall(context)) return false;
	}

	if (it->first != "semicolon") return false;
	nextToken();

	rollbackGraphNode();
	return true;
}

FT LL::ForExp(const std::string &context) {
	bool oneFlag = false;
	std::string _temp;

	if (it->first == "lpar" || it->first == "opinc" || it->first == "num" || it->first == "char" || it->first == "id") {
		nextGraphState(0);
		generateString("E");

		FT ERes = Expr(context);

		if (!ERes.first) oneFlag = true;
		else oneFlag = false;

		_temp = ERes.second;
	}

	if (it->first == "semicolon") {
		nextToken();

		rollbackGraphNode();
		if (oneFlag) return {true, "1"};
		return {true, _temp};
	}

	return {false, ""};
}

bool LL::ForLoop(const std::string &context) {
	if (it->first == "opinc") {
		nextToken();

		if (it->first != "id") return false;
		std::string _temp = it->second;
		nextToken();

		auto p = checkVar(context, _temp);
		generateAtom(context, "ADD", p, "1", p);

		if (it->first != "rpar") return false;
		nextToken();

		nextGraphState(0);
		generateString("opinc " + _temp);

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	if (it->first == "id") {
		nextGraphState(1);
		generateString("AssignOrCall");

		if (!AssignOrCall(context)) return false;

		if (it->first != "rpar") return false;
		nextToken();

		rollbackGraphNode();
		return true;
	}

	if (it->first != "rpar") return false;
	nextToken();

	rollbackGraphNode();
	return true;
}

bool LL::IfOp(const std::string &context) {
	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("lpar E");

	auto ERes = Expr(context);
	if (!ERes.first) return false;

	if (it->first != "rpar") return false;
	nextToken();

	auto l1 = newLabel();
	generateAtom(context, "EQ", ERes.second, "0", "L" + l1);

	nextGraphState(1);
	generateString("rpar Stmt");

	if (!Stmt(context)) return false;

	auto l2 = newLabel();
	generateAtom(context, "JMP", "", "", "L" + l2);
	generateAtom(context, "LBL", "", "", "L" + l1);

	nextGraphState(0);
	generateString("ElsePart");

	if (it->first == "kwelse") {
		if (!ElsePart(context)) return false;

		generateAtom(context, "LBL", "", "", "L" + l2);

		rollbackGraphNode();
		return true;
	}

	generateAtom(context, "LBL", "", "", "L" + l2);

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::ElsePart(const std::string &context) {

	nextToken();

	nextGraphState(0);
	generateString("kwelse Stmt");

	if (!Stmt(context)) return false;

	rollbackGraphNode();
	return true;
}

bool LL::SwitchOp(const std::string &context) {
	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("lpar E");

	auto ERes = Expr(context);
	if (!ERes.first) return false;

	if (it->first != "rpar") return false;
	nextToken();

	if (it->first != "lbrace") return false;
	nextToken();

	nextGraphState(1);
	generateString("rpar lbrace Cases");

	auto end = newLabel();
	if (!Cases(context, ERes.second, end)) return false;

	if (it->first != "rbrace") return false;
	nextToken();

	generateAtom(context, "LBL", "", "", "L" + end);

	nextGraphState(0);
	generateString("rbrace");

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::Cases(const std::string &context, const std::string &p, const std::string &end) {
	nextGraphState(1);
	generateString("ACase");

	auto ACaseRes = ACase(context, p, end);
	if (!ACaseRes.first) return false;

	nextGraphState(0);
	generateString("Cases'");

	if (!CasesList(context, p, end, ACaseRes.second)) return false;

	rollbackGraphNode();
	return true;
}

bool LL::CasesList(const std::string &context, const std::string &p, const std::string &end, const std::string &def) {

	if (it->first == "kwcase" || it->first == "kwdefault") {
		nextGraphState(1);
		generateString("ACase");

		FT ACaseRes = ACase(context, p, end);
		if (!ACaseRes.first) return false;

		if ((stoi(def) >= 0) && (stoi(ACaseRes.second) >= 0)) {
			generateAtom("SYSTEM", "ERROR", "ERROR", "ERROR", "ERROR");
		}

		auto maxDef = stoi(def) < stoi(ACaseRes.second) ? ACaseRes.second : def;

		nextGraphState(0);
		generateString("Cases'");

		if (!CasesList(context, p, end, maxDef)) return false;

		rollbackGraphNode();
		return true;
	}

	std::string q = stoi(def) >= 0 ? def : end;
	generateAtom(context, "JMP", "", "", "L" + q);

	rollbackGraphNode();
	return true;
}

FT LL::ACase(const std::string &context, const std::string &p, const std::string &end) {
	if (it->first == "kwcase") {
		nextToken();

		if (it->first != "num") return {false, ""};
		std::string _temp = it->second;
		nextToken();

		std::string next = newLabel();
		generateAtom(context, "NE", p, _temp, "L" + next);

		if (it->first != "colon") return {false, ""};
		nextToken();

		nextGraphState(0);
		generateString("kwcase " + _temp + "colon StmtList");

		if (!StmtList(context)) return {false, ""};

		generateAtom(context, "JMP", "", "", "L" + end);
		generateAtom(context, "LBL", "", "", "L" + next);

		rollbackGraphNode();
		return {true, "-1"};
	}

	if (it->first == "kwdefault") {
		nextToken();

		if (it->first != "colon") return {false, ""};
		nextToken();

		auto next = newLabel();
		auto def = newLabel();

		generateAtom(context, "JMP", "", "", "L" + next);
		generateAtom(context, "LBL", "", "", "L" + def);

		nextGraphState(0);
		generateString("kwdefault colon StmtList");

		if (!StmtList(context)) return {false, ""};

		generateAtom(context, "JMP", "", "", "L" + end);
		generateAtom(context, "LBL", "", "", "L" + next);

		rollbackGraphNode();
		return {true, def};
	}

	return {false, ""};
}

bool LL::InOp(const std::string &context) {
	if (it->first != "id") return false;
	std::string _temp = it->second;
	nextToken();

	if (it->first != "semicolon") return false;
	nextToken();

	auto p = checkVar(context, _temp);
	generateAtom(context, "IN", "", "", p);

	nextGraphState(0);
	generateString(_temp + " semicolon");

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::OutOp(const std::string &context) {

	nextGraphState(1);
	generateString("OutOp'");

	if (!OutOpList(context)) return false;

	if (it->first != "semicolon") return false;
	nextToken();

	nextGraphState(0);
	generateString("semicolon");

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::OutOpList(const std::string &context) {
	if (it->first == "str") {
		std::string _temp = it->second;
		nextToken();

		nextGraphState(0);
		generateString("\"" + _temp + "\"");

		generateAtom(context, "OUT", "", "", "\"" + _temp + "\"");
		rollbackGraphNode();
	} else {

		nextGraphState(0);
		generateString("E");

		auto ERes = Expr(context);
		if (!ERes.first) return false;

		generateAtom(context, "OUT", "", "", ERes.second);
	}

	rollbackGraphNode();
	return true;
}

FT LL::Type(const std::string &context) {
	if (it->first == "eof") return {false, ""};

	if (it->first == "kwint" || it->first == "kwchar") {

		auto temp = it->first;
		nextToken();

		nextGraphState(0);
		generateString(temp);

		rollbackGraphNode();
		rollbackGraphNode();
		return {true, temp};
	}

	return {false, ""};
}

bool LL::DeclareVarList(const std::string &context, const std::string &type) {
	if (it->first == "eof") return false;

	if (it->first == "comma") {
		nextToken();

		if (it->first != "id") return false;
		auto temp = it->second;

		nextGraphState(1);
		generateString("comma " + it->second + " InitVar");

		nextToken();

		if (!InitVar(context, type, temp)) return false;

		nextGraphState(0);
		generateString("DeclareVarList");

		if (!DeclareVarList(context, type)) return false;
	}

	rollbackGraphNode();
	return true;
}

bool LL::InitVar(const std::string &context, const std::string &r, const std::string &s) {
	if (it->first == "eof") return false;

	if (it->first == "opassign") {
		nextToken();

		if (it->first == "num" || it->first == "char") {

			addVar(s, context, r, it->second);

			nextGraphState(0);
			generateString("opassign " + it->second);

			nextToken();

			rollbackGraphNode();
			rollbackGraphNode();
			return true;
		}

		return false;
	}

	addVar(s, context, r);
	rollbackGraphNode();
	return true;
}

FT LL::ParamList(const std::string &context) {
	if (it->first == "eof") return {false, ""};

	if (it->first == "kwint" || it->first == "kwchar") {
		nextGraphState(1);
		generateString("Type");

		auto TypeResult = Type(context);

		if (it->first != "id") return {false, ""};
		addVar(it->second, context, TypeResult.second);

		nextGraphState(0);
		generateString(it->second + " ParamList'");

		nextToken();

		FT ParLLRes = ParamListList(context);
		if (!ParLLRes.first) return {false, ""};

		rollbackGraphNode();
		return {true, std::to_string(stoi(ParLLRes.second) + 1)};
	}

	rollbackGraphNode();
	return {true, "0"};
}

FT LL::ParamListList(const std::string &context) {
	if (it->first == "eof") return {false, ""};

	if (it->first == "comma") {
		nextToken();

		if (it->first == "kwint" || it->first == "kwchar") {
			nextGraphState(1);
			generateString("comma Type");

			auto TypeResult = Type(context);

			if (it->first != "id") return {false, ""};
			addVar(it->second, context, TypeResult.second);

			nextGraphState(0);
			generateString(it->second + " ParamList'");

			nextToken();

			FT ParLLRes = ParamListList(context);
			if (!ParLLRes.first) return {false, ""};

			rollbackGraphNode();
			return {true, std::to_string(stoi(ParLLRes.second) + 1)};
		}
	}

	rollbackGraphNode();
	return {true, "0"};
}

// Expressions check
FT LL::Expr(const std::string &context) {

	nextGraphState(0);
	generateString("E7");

	auto E7Result = Expr7(context);
	if (!E7Result.first) return {false, ""};

	rollbackGraphNode();
	return {true, E7Result.second};
}

FT LL::Expr7(const std::string &context) {

	nextGraphState(1);
	generateString("E6");

	auto E6Result = Expr6(context);
	if (!E6Result.first) return {false, ""};

	nextGraphState(0);
	generateString("E7'");

	auto E7ListResult = Expr7List(context, E6Result.second);
	if (!E7ListResult.first) return {false, ""};

	rollbackGraphNode();
	return {true, E7ListResult.second};
}

FT LL::Expr7List(const std::string &context, const std::string &funcID) {
	if (it->first == "opor") {
		nextToken();

		nextGraphState(1);
		generateString("opor E6");

		auto E6Result = Expr6(context);
		if (!E6Result.first) return {false, ""};

		auto s = alloc(context);
		generateAtom(context, "OR", funcID, E6Result.second, s);

		nextGraphState(0);
		generateString("E7'");

		auto E7ListResult = Expr7List(context, s);
		if (!E7ListResult.first) return {false, ""};

		rollbackGraphNode();
		return {true, E7ListResult.second};
	}

	rollbackGraphNode();
	return {true, funcID};
}

FT LL::Expr6(const std::string &context) {
	nextGraphState(1);
	generateString("E5");

	auto E5Result = Expr5(context);
	if (!E5Result.first) return {false, ""};

	nextGraphState(0);
	generateString("E6'");

	auto E6ListResult = Expr6List(context, E5Result.second);
	if (!E6ListResult.first) return {false, ""};

	rollbackGraphNode();
	return {true, E6ListResult.second};
}

FT LL::Expr6List(const std::string &context, const std::string &funcID) {
	if (it->first == "opand") {
		nextToken();

		nextGraphState(1);
		generateString("opand E5");

		auto E5Result = Expr5(context);
		if (!E5Result.first) return {false, ""};

		auto s = alloc(context);
		generateAtom(context, "AND", funcID, E5Result.second, s);

		nextGraphState(0);
		generateString("E6'");

		auto E6ListResult = Expr6List(context, s);
		if (!E6ListResult.first) return {false, ""};

		rollbackGraphNode();
		return {true, E6ListResult.second};
	}

	rollbackGraphNode();
	return {true, funcID};
}

FT LL::Expr5(const std::string &context) {

	nextGraphState(1);
	generateString("E4");

	auto E4Result = Expr4(context);
	if (!E4Result.first) return {false, ""};

	nextGraphState(0);
	generateString("E5'");

	auto E5ListResult = Expr5List(context, E4Result.second);
	if (!E5ListResult.first) return {false, ""};

	rollbackGraphNode();
	return {true, E5ListResult.second};
}

FT LL::Expr5List(const std::string &context, const std::string &funcID) {
	if (it->first == "opeq") {
		nextToken();

		nextGraphState(0);
		generateString("opeq E4");

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, ""};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "EQ", funcID, E4Result.second, "L" + l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", "L" + l);

		rollbackGraphNode();
		return {true, s};

	} else if (it->first == "opne") {
		nextToken();

		nextGraphState(0);
		generateString("opne E4");

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, ""};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "NE", funcID, E4Result.second, "L" + l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", "L" + l);

		rollbackGraphNode();
		return {true, s};
	} else if (it->first == "ople") {
		nextToken();

		nextGraphState(0);
		generateString("ople E4");

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, ""};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "LE", funcID, E4Result.second, "L" + l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", "L" + l);

		rollbackGraphNode();
		return {true, s};
	} else if (it->first == "opgt") {
		nextToken();

		nextGraphState(0);
		generateString("opgt E4");

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, ""};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "GT", funcID, E4Result.second, "L" + l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", "L" + l);

		rollbackGraphNode();
		return {true, s};
	} else if (it->first == "opge") {
		nextToken();

		nextGraphState(0);
		generateString("opge E4");

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, ""};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "GE", funcID, E4Result.second, "L" + l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", "L" + l);

		rollbackGraphNode();
		return {true, s};
	} else if (it->first == "oplt") {
		nextToken();

		nextGraphState(0);
		generateString("oplt E4");

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, ""};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "LT", funcID, E4Result.second, "L" + l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", "L" + l);

		rollbackGraphNode();
		return {true, s};
	}

	rollbackGraphNode();
	return {true, funcID};
}

FT LL::Expr4(const std::string &context) {

	nextGraphState(1);
	generateString("E3");

	auto E3Result = Expr3(context);
	if (!E3Result.first) return {false, ""};

	nextGraphState(0);
	generateString("E4'");

	auto E4ListResult = Expr4List(context, E3Result.second);
	if (!E4ListResult.first) return {false, ""};

	rollbackGraphNode();
	return {true, E4ListResult.second};
}

FT LL::Expr4List(const std::string &context, const std::string &funcID) {
	if (it->first == "opplus") {
		nextToken();

		nextGraphState(1);
		generateString("opplus E3");

		auto E3Result = Expr3(context);
		if (!E3Result.first) return {false, ""};

		auto s = alloc(context);
		generateAtom(context, "ADD", funcID, E3Result.second, s);

		nextGraphState(0);
		generateString("E4'");

		auto E4ListResult = Expr4List(context, s);
		if (!E4ListResult.first) return {false, ""};

		rollbackGraphNode();
		return {true, E4ListResult.second};

	} else if (it->first == "opminus") {
		nextToken();

		nextGraphState(1);
		generateString("opminus E3");

		auto E3Result = Expr3(context);
		if (!E3Result.first) return {false, ""};

		auto s = alloc(context);
		generateAtom(context, "SUB", funcID, E3Result.second, s);

		nextGraphState(0);
		generateString("E4'");

		auto E4ListResult = Expr4List(context, s);
		if (!E4ListResult.first) return {false, ""};

		rollbackGraphNode();
		return {true, E4ListResult.second};
	}

	rollbackGraphNode();
	return {true, funcID};
}

FT LL::Expr3(const std::string &context) {

	nextGraphState(1);
	generateString("E2");

	auto E2Result = Expr2(context);
	if (!E2Result.first) return {false, ""};

	nextGraphState(0);
	generateString("E3'");

	auto E3ListResult = Expr3List(context, E2Result.second);
	if (!E3ListResult.first) return {false, ""};

	rollbackGraphNode();
	return {true, E3ListResult.second};

}

FT LL::Expr3List(const std::string &context, const std::string &funcID) {
	if (it->first == "opmul") {
		nextToken();

		nextGraphState(1);
		generateString("opmul E2");

		auto E2Result = Expr2(context);
		if (!E2Result.first) return {false, ""};

		auto s = alloc(context);
		generateAtom(context, "MUL", funcID, E2Result.second, s);

		nextGraphState(0);
		generateString("E3'");

		auto E3ListResult = Expr3List(context, s);
		if (!E3ListResult.first) return {false, ""};

		rollbackGraphNode();
		return {true, E3ListResult.second};
	}

	rollbackGraphNode();
	return {true, funcID};
}

FT LL::Expr2(const std::string &context) {
	if (it->first == "opnot") {
		nextToken();

		nextGraphState(0);
		generateString("opnot E1");

		auto E1Result = Expr1(context);
		if (!E1Result.first) return {false, ""};

		auto r = alloc(context);
		generateAtom(context, "NOT", E1Result.second, "", r);

		rollbackGraphNode();
		return {true, r};
	} else {

		nextGraphState(0);
		generateString("E1");

		auto E1Result = Expr1(context);
		if (!E1Result.first) return {false, ""};

		rollbackGraphNode();
		return {true, E1Result.second};
	}
}

FT LL::Expr1(const std::string &context) {
	if (it->first == "opinc") {
		nextToken();

		if (it->first != "id") return {false, ""};

		auto q = checkVar(context, it->second);
		generateAtom(context, "ADD", q, "1", q);

		nextGraphState(0);
		generateString("opinc " + it->second);
		nextToken();

		rollbackGraphNode();
		rollbackGraphNode();
		return {true, q};
	} else if (it->first == "lpar") {
		nextToken();

		nextGraphState(1);
		generateString("lpar E");

		auto EResult = Expr(context);
		if (!EResult.first) return {false, ""};

		nextGraphState(0);
		generateString("rpar");

		if (it->first != "rpar") return {false, ""};
		nextToken();

		rollbackGraphNode();
		rollbackGraphNode();
		return {true, EResult.second};
	} else if (it->first == "num") {
		auto val = it->second;

		nextGraphState(0);
		generateString(val);
		nextToken();

		rollbackGraphNode();
		rollbackGraphNode();
		return {true, val};
	} else if (it->first == "char") {
		auto val = it->second;

		nextGraphState(0);
		generateString(val);
		nextToken();

		rollbackGraphNode();
		rollbackGraphNode();
		return {true, val};
	} else if (it->first == "id") {
		auto r = it->second;

		nextGraphState(0);
		generateString(r + " E1'");

		nextToken();

		auto E1ListResult = Expr1List(context, r);
		if (!E1ListResult.first) return {false, ""};

		rollbackGraphNode();
		return {true, E1ListResult.second};
	}

	return {false, ""};
}

FT LL::Expr1List(const std::string &context, const std::string &funcID) {
	if (it->first == "lpar") {
		nextToken();

		nextGraphState(1);
		generateString("lpar ArgList");

		auto ArgListResult = ArgList(context);
		if (!ArgListResult.first) return {false, ""};

		nextGraphState(0);
		generateString("rpar");

		if (it->first != "rpar") return {false, ""};
		nextToken();

		auto s = checkFunc(funcID, ArgListResult.second);
		auto r = alloc(context);

		generateAtom(context, "CALL", s, "", r);

		rollbackGraphNode();
		rollbackGraphNode();
		return {true, r};
	}

	if (it->first == "opinc") {
		nextToken();

		auto s = checkVar(context, funcID);
		auto r = alloc(context);

		generateAtom(context, "MOV", s, "", r);
		generateAtom(context, "ADD", s, "1", s);

		nextGraphState(0);
		generateString("opinc");
		rollbackGraphNode();
		rollbackGraphNode();
		return {true, r};
	}

	auto q = checkVar(context, funcID);

	rollbackGraphNode();
	return {true, q};
}

FT LL::ArgList(const std::string &context) {
	if (it->first == "lpar" || it->first == "opinc" || it->first == "num" || it->first == "char" || it->first == "id") {

		nextGraphState(1);
		generateString(it->first + " E");

		auto ERes = Expr(context);
		if (!ERes.first) return {false, ""};

		nextGraphState(0);
		generateString("ArgList'");

		FT ArgListListResult = ArgListList(context);
		if (!ArgListListResult.first) return {false, ""};

		generateAtom(context, "PARAM", "", "", ERes.second);

		rollbackGraphNode();
		return {true, std::to_string(stoi(ArgListListResult.second) + 1)};
	}

	rollbackGraphNode();
	return {true, "0"};
}

FT LL::ArgListList(const std::string &context) {
	if (it->first == "comma") {
		nextToken();

		if (it->first == "lpar" || it->first == "opinc" || it->first == "num" || it->first == "char" ||
		    it->first == "id") {

			nextGraphState(1);
			generateString("comma E");

			auto ERes = Expr(context);
			if (!ERes.first) return {false, ""};

			nextGraphState(0);
			generateString("ArgList'");

			FT ArgListListResult = ArgListList(context);
			if (!ArgListListResult.first) return {false, ""};

			generateAtom(context, "PARAM", "", "", ERes.second);

			rollbackGraphNode();
			return {true, std::to_string(stoi(ArgListListResult.second) + 1)};
		} else {
			return {false, ""};
		}
	}

	rollbackGraphNode();
	return {true, "0"};
}