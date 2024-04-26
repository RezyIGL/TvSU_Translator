#include "Translator.h"
#include <algorithm>

// prints graph
void LL::_printGraph() {
	myStream.open(_graph);

	myStream << "StmtList" << std::endl;

	for (auto &i: outputVector) {
		myStream << i << std::endl;
	}

	myStream.close();
}

// This thing generates Atoms
bool LL::generateAtoms() {
	if (AtomsGenerated) return true;

	if (!atoms.empty() && atoms.front().text == "ERROR") {
		AtomsGenerated = true;
		SemanticError = true;

		std::cout << "##########################" << std::endl;
		std::cout << "      Semantic Error!     " << std::endl;
		std::cout << "##########################" << std::endl << std::endl;

		return false;
	}

	for (const auto &i: AtomicMap) {
		for (const auto &j: i.second) {
			sortedAtomsVector.emplace_back(j);
		}
	}

	struct {
		bool operator()(VarOrFunc &a, VarOrFunc &b) const { return a.cnt < b.cnt; };
	} customLess;
	std::sort(sortedAtomsVector.begin(), sortedAtomsVector.end(), customLess);

	for (const auto &o: sortedAtomsVector) {
		if (o.name == "main") entryPoint = std::to_string(o.cnt);
	}

	for (const auto &i : atoms) {
		if (i.context == entryPoint) isUsed = true;
	}

	AtomsGenerated = true;

	return true;
}

// This thing clears files
void LL::clearStream(const std::string &stream) {
	if (stream.empty()) return;
	myStream.open(stream);
	myStream.clear();
	myStream.close();
}

// Generates string
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

// Translator analyzer check functions
// Semantic analyzer functions
std::string LL::newLabel() {
	return std::to_string(LabelCnt++);
}

std::string LL::alloc(const std::string &scope) {
	std::string temp = addVar("$T" + std::to_string(NewVarCnt++), scope, "kwint", "0");
	return "'" + temp + "'";
}

std::string
LL::addVar(const std::string &name, const std::string &scope, const std::string &type, const std::string &init) {
	if (AtomicMap.count(scope)) {
		for (const auto &i: AtomicMap[scope]) {
			if (i.name == name) {
				return "Error";
			}
		}
	}

	VarOrFunc temp = {name, scope, type, init, "var", AtomicMapCnt++};
	AtomicMap[scope].emplace_back(temp);

	return std::to_string(temp.cnt);
}

std::string LL::addFunc(const std::string &name, const std::string &type, const std::string &length) {
	for (const auto &i: AtomicMap["-1"]) {
		if (i.name == name) {
			return "ERROR";
		}
	}

	VarOrFunc temp = {name, "-1", type, length.empty() ? "0" : length, "func", AtomicMapCnt++};
	AtomicMap["-1"].emplace_back(temp);

	return std::to_string(temp.cnt);
}

std::string LL::checkVar(const std::string &name) {

	auto contextIter = contextVector.rbegin();

	while (contextIter != contextVector.rend()) {
		for (const auto &i: AtomicMap[*contextIter]) {
			if (i.name == name && i.kind == "var") return "'" + std::to_string(i.cnt) + "'";
			else if (i.name == name && i.type != "var") return "ERROR";
		}

		contextIter++;
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
