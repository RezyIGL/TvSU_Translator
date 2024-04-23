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

	for (const auto &i: atoms) {
		myStream << i.context << ": ("
		         << i.text << ","
		         << i.first << ","
		         << i.second << ","
		         << i.third << ")" << std::endl;
		atoms.pop_back();
	}

	myStream << "\n=============================================\nName : Code : Class : Type : Init : Scope \n\n";

	if (!atoms.empty() && atoms.front().text == "ERROR") {
		std::cout << "===========[Your code contains Semantic Errors!]===========" << std::endl;
		myStream.close();
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

	for (const auto &i: sortedAtomsVector) {
		myStream << i.name << " : '" << i.cnt << "' : " << i.kind << " : " << i.type << " : " << i.init
		         << " : " << i.scope << std::endl;
	}

	myStream.close();
	return true;
}

void LL::printASMCode() {
	myStream.open(_asmOut);

	for (const auto &i : sortedAtomsVector) {
		if (i.kind == "var") {
			if (i.type == "kwint") {
				myStream << i.name << ": db ";
				if (i.init.empty()) myStream << "0H" << std::endl;
				else myStream << i.init << "H" << std::endl;
			} else {
				myStream << i.name << ": dw ";
				if (i.init.empty()) myStream << "\"\"" << std::endl;
				else myStream << "\"" << i.init << "\"" << std::endl;
			}
		}
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
	graphIterator = states.begin();

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

void LL::nextGraphState(const int &state) {
//	states.emplace_back(state);
//	graphIterator = states.end() - 1;
}

void LL::rollbackGraphNode() {
//	graphIterator = (graphIterator > states.begin()) ? graphIterator - 1 : states.begin();
//	states.pop_back();
}

// MiniC grammar check
bool LL::StmtList(const std::string &context) {
	if (it->first == "eof") return true;

	bool result = Stmt(context);
	if (!result) return true;

	bool tailResult = StmtList(context);
	if (!tailResult) return false;

	return true;
}

bool LL::Stmt(const std::string &context) {
	if (it->first == "eof") return false;

	if (it->first == "kwint" || it->first == "kwchar") {
		if (!DeclareStmt(context)) return false;

		return true;
	}

	if (it->first == "semicolon") {
		nextToken();

		return true;
	}

	if (context == "-1") return false;

	if (it->first == "id") {
		if (!AssignOrCallOp(context)) return false;

		return true;
	}

	if (it->first == "kwwhile") {
		nextToken();

		if (!WhileOp(context)) return false;

		return true;
	}

	if (it->first == "kwfor") {
		nextToken();

		if (!ForOp(context)) return false;

		return true;
	}

	if (it->first == "kwif") {
		nextToken();

		if (!IfOp(context)) return false;
		return true;
	}

	if (it->first == "kwswitch") {
		nextToken();

		if (!SwitchOp(context)) return false;

		return true;
	}

	if (it->first == "kwin") {
		nextToken();

		if (!InOp(context)) return false;

		return true;
	}

	if (it->first == "kwout") {
		nextToken();

		if (!OutOp(context)) return false;

		return true;
	}

	if (it->first == "lbrace") {
		nextToken();

		bool result = StmtList(context);
		if (!result) return false;

		if (it->first != "rbrace") return false;
		nextToken();

		return true;
	}

	if (it->first == "kwreturn") {
		nextToken();

		auto EResult = Expr(context);
		if (!EResult.first) return false;

		generateAtom(context, "RET", "", "", EResult.second);

		if (it->first != "semicolon") return false;
		nextToken();

		return true;
	}

	return false;
}

bool LL::DeclareStmt(const std::string &context) {
	auto TypeResult = Type(context);
	if (!TypeResult.first) return false;

	if (it->first != "id") return false;
	auto temp = it->second;
	nextToken();

	if (!DeclareStmtList(context, TypeResult.second, temp)) return false;

	return true;
}

bool LL::DeclareStmtList(const std::string &context, const std::string &type, const std::string &name) {
	if (it->first == "eof") return false;

	if (it->first == "lpar") {
		if (stoi(context) > -1) return false;
		nextToken();

		auto ParamListRes = ParamList(context);
		if (!ParamListRes.first) return false;

		if (it->first != "rpar") return false;
		nextToken();

		auto TC = addFunc(name, type, ParamListRes.second);

		if (it->first != "lbrace") return false;
		nextToken();

		if (!StmtList(TC)) return false;

		if (it->first != "rbrace") return false;
		nextToken();

		generateAtom(context, "RET", "", "", "0");

		return true;
	} else if (it->first == "opassign") {
		nextToken();

		if (it->first == "num") {
			addVar(name, context, type, it->second);
			nextToken();

			auto DecVarListRes = DeclareVarList(context, type);
			if (!DecVarListRes) return false;

			if (it->first != "semicolon") return false;
			nextToken();

			return true;
		} else if (it->first == "char") {
			addVar(name, context, type, it->second);
			nextToken();

			auto DecVarListRes = DeclareVarList(context, type);
			if (!DecVarListRes) return false;

			if (it->first != "semicolon") return false;
			nextToken();

			return true;
		} else {
			return false;
		}
	} else {
		addVar(name, context, type);

		if (!DeclareVarList(context, type)) return false;

		if (it->first != "semicolon") return false;
		nextToken();

		return true;
	}
}

bool LL::AssignOrCallOp(const std::string &context) {
	if (!AssignOrCall(context)) return false;

	if (it->first != "semicolon") return false;
	nextToken();

	return true;
}

bool LL::AssignOrCall(const std::string &context) {
	if (it->first != "id") return false;
	auto temp = it->second;
	nextToken();

	if (!AssignOrCallList(context, temp)) return false;

	return true;
}

bool LL::AssignOrCallList(const std::string &context, const std::string &name) {
	if (it->first == "opassign") {
		nextToken();

		auto ERes = Expr(context);
		if (!ERes.first) return false;

		auto r = checkVar(context, name);
		generateAtom(context, "MOV", ERes.second, "", r);

		return true;
	}

	if (it->first == "lpar") {
		nextToken();

		auto ArgListRes = ArgList(context);
		if (!ArgListRes.first) return false;

		if (it->first != "rpar") return false;
		nextToken();

		auto q = checkFunc(name, ArgListRes.second);
		auto r = alloc(context);
		generateAtom(context, "CALL", q, "", r);

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

	auto ERes = Expr(context);
	if (!ERes.first) return false;

	generateAtom(context, "EQ", ERes.second, "0", "L" + l2);

	if (it->first != "rpar") return false;
	nextToken();

	if (!Stmt(context)) return false;

	generateAtom(context, "JMP", "", "", "L" + l1);
	generateAtom(context, "LBL", "", "", "L" + l2);

	return true;
}

bool LL::ForOp(const std::string &context) {

	auto l1 = newLabel();
	auto l2 = newLabel();
	auto l3 = newLabel();
	auto l4 = newLabel();

	if (it->first != "lpar") return false;
	nextToken();

	if (!ForInit(context)) return false;

	generateAtom(context, "LBL", "", "", "L" + l1);

	FT ForExpRes = ForExp(context);
	if (!ForExpRes.first) return false;

	generateAtom(context, "EQ", ForExpRes.second, "0", "L" + l4);
	generateAtom(context, "JMP", "", "", "L" + l3);
	generateAtom(context, "LBL", "", "", "L" + l2);

	if (!ForLoop(context)) return false;

	generateAtom(context, "JMP", "", "", "L" + l1);
	generateAtom(context, "LBL", "", "", "L" + l3);

	if (!Stmt(context)) return false;

	generateAtom(context, "JMP", "", "", "L" + l2);
	generateAtom(context, "LBL", "", "", "L" + l4);

	return true;
}

bool LL::ForInit(const std::string &context) {
	if (it->first == "id") {
		if (!AssignOrCall(context)) return false;
	}

	if (it->first != "semicolon") return false;
	nextToken();

	return true;
}

FT LL::ForExp(const std::string &context) {
	bool oneFlag = false;
	std::string _temp;

	if (it->first == "lpar" || it->first == "opinc" || it->first == "num" || it->first == "char" || it->first == "id") {
		FT ERes = Expr(context);

		if (!ERes.first) oneFlag = true;
		else oneFlag = false;

		_temp = ERes.second;
	}

	if (it->first == "semicolon") {
		nextToken();

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

		return true;
	}

	if (it->first == "id") {
		if (!AssignOrCall(context)) return false;

		if (it->first != "rpar") return false;
		nextToken();

		return true;
	}

	if (it->first != "rpar") return false;
	nextToken();

	return true;
}

bool LL::IfOp(const std::string &context) {
	if (it->first != "lpar") return false;
	nextToken();

	auto ERes = Expr(context);
	if (!ERes.first) return false;

	if (it->first != "rpar") return false;
	nextToken();

	auto l1 = newLabel();
	generateAtom(context, "EQ", ERes.second, "0", "L" + l1);

	if (!Stmt(context)) return false;

	auto l2 = newLabel();
	generateAtom(context, "JMP", "", "", "L" + l2);
	generateAtom(context, "LBL", "", "", "L" + l1);

	if (it->first == "kwelse") {
		nextToken();

		if (!ElsePart(context)) return false;
	}

	generateAtom(context, "LBL", "", "", "L" + l2);

	return true;
}

bool LL::ElsePart(const std::string &context) {

	if (!Stmt(context)) return false;

	return true;
}

bool LL::SwitchOp(const std::string &context) {
	if (it->first != "lpar") return false;
	nextToken();

	auto ERes = Expr(context);
	if (!ERes.first) return false;

	if (it->first != "rpar") return false;
	nextToken();

	if (it->first != "lbrace") return false;
	nextToken();

	auto end = newLabel();
	if (!Cases(context, ERes.second, end)) return false;

	if (it->first != "rbrace") return false;
	nextToken();

	generateAtom(context, "LBL", "", "", "L" + end);

	return true;
}

bool LL::Cases(const std::string &context, const std::string &p, const std::string &end) {
	auto ACaseRes = ACase(context, p, end);
	if (!ACaseRes.first) return false;

	if (!CasesList(context, p, end, ACaseRes.second)) return false;

	return true;
}

bool LL::CasesList(const std::string &context, const std::string &p, const std::string &end, const std::string &def) {

	if (it->first == "kwcase" || it->first == "kwdefault") {
		FT ACaseRes = ACase(context, p, end);
		if (!ACaseRes.first) return false;

		if ((stoi(def) >= 0) && (stoi(ACaseRes.second) >= 0)) {
			generateAtom("SYSTEM", "ERROR", "ERROR", "ERROR", "ERROR");
		}

		auto maxDef = stoi(def) < stoi(ACaseRes.second) ? ACaseRes.second : def;

		if (!CasesList(context, p, end, maxDef)) return false;

		return true;
	}

	std::string q = stoi(def) >= 0 ? def : end;
	generateAtom(context, "JMP", "", "", "L" + q);

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

		if (!StmtList(context)) return {false, ""};

		generateAtom(context, "JMP", "", "", "L" + end);
		generateAtom(context, "LBL", "", "", "L" + next);

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

		if (!StmtList(context)) return {false, ""};

		generateAtom(context, "JMP", "", "", "L" + end);
		generateAtom(context, "LBL", "", "", "L" + next);

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

	return true;
}

bool LL::OutOp(const std::string &context) {
	if (!OutOpList(context)) return false;

	if (it->first != "semicolon") return false;
	nextToken();

	return true;
}

bool LL::OutOpList(const std::string &context) {
	if (it->first == "str") {
		std::string _temp = it->second;
		nextToken();

		generateAtom(context, "OUT", "", "", "\"" + _temp + "\"");
	} else {
		auto ERes = Expr(context);
		if (!ERes.first) return false;

		generateAtom(context, "OUT", "", "", ERes.second);
	}

	return true;
}

FT LL::Type(const std::string &context) {
	if (it->first == "eof") return {false, ""};

	if (it->first == "kwint" || it->first == "kwchar") {

		auto temp = it->first;
		nextToken();

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
		nextToken();

		if (!InitVar(context, type, temp)) return false;

		if (!DeclareVarList(context, type)) return false;
	}

	return true;
}

bool LL::InitVar(const std::string &context, const std::string &r, const std::string &s) {
	if (it->first == "eof") return false;

	if (it->first == "opassign") {
		nextToken();

		if (it->first == "num" || it->first == "char") {

			addVar(s, context, r, it->second);
			nextToken();

			return true;
		}

		return false;
	}

	addVar(s, context, r);
	return true;
}

FT LL::ParamList(const std::string &context) {
	if (it->first == "eof") return {false, ""};

	if (it->first == "kwint" || it->first == "kwchar") {
		auto TypeResult = Type(context);

		if (it->first != "id") return {false, ""};
		addVar(it->second, context, TypeResult.second);
		nextToken();

		FT ParLLRes = ParamListList(context);
		if (!ParLLRes.first) return {false, ""};

		return {true, std::to_string(stoi(ParLLRes.second) + 1)};
	}

	return {true, "0"};
}

FT LL::ParamListList(const std::string &context) {
	if (it->first == "eof") return {false, ""};

	if (it->first == "comma") {
		nextToken();

		if (it->first == "kwint" || it->first == "kwchar") {
			auto TypeResult = Type(context);

			if (it->first != "id") return {false, ""};
			addVar(it->second, context, TypeResult.second);
			nextToken();

			FT ParLLRes = ParamListList(context);
			if (!ParLLRes.first) return {false, ""};

			return {true, std::to_string(stoi(ParLLRes.second) + 1)};
		}
	}

	return {true, "0"};
}

// Expressions check
FT LL::Expr(const std::string &context) {
	auto E7Result = Expr7(context);
	if (!E7Result.first) return {false, ""};

	return {true, E7Result.second};
}

FT LL::Expr7(const std::string &context) {
	auto E6Result = Expr6(context);
	if (!E6Result.first) return {false, ""};

	auto E7ListResult = Expr7List(context, E6Result.second);
	if (!E7ListResult.first) return {false, ""};

	return {true, E7ListResult.second};
}

FT LL::Expr7List(const std::string &context, const std::string &funcID) {
	if (it->first == "opor") {
		nextToken();

		auto E6Result = Expr6(context);
		if (!E6Result.first) return {false, ""};

		auto s = alloc(context);
		generateAtom(context, "OR", funcID, E6Result.second, s);

		auto E7ListResult = Expr7List(context, s);
		if (!E7ListResult.first) return {false, ""};

		return {true, E7ListResult.second};
	}

	return {true, funcID};
}

FT LL::Expr6(const std::string &context) {
	auto E5Result = Expr5(context);
	if (!E5Result.first) return {false, ""};

	auto E6ListResult = Expr6List(context, E5Result.second);
	if (!E6ListResult.first) return {false, ""};

	return {true, E6ListResult.second};
}

FT LL::Expr6List(const std::string &context, const std::string &funcID) {
	if (it->first == "opand") {
		nextToken();

		auto E5Result = Expr5(context);
		if (!E5Result.first) return {false, ""};

		auto s = alloc(context);
		generateAtom(context, "AND", funcID, E5Result.second, s);

		auto E6ListResult = Expr6List(context, s);
		if (!E6ListResult.first) return {false, ""};

		return {true, E6ListResult.second};
	}

	return {true, funcID};
}

FT LL::Expr5(const std::string &context) {
	auto E4Result = Expr4(context);
	if (!E4Result.first) return {false, ""};

	auto E5ListResult = Expr5List(context, E4Result.second);
	if (!E5ListResult.first) return {false, ""};

	return {true, E5ListResult.second};
}

FT LL::Expr5List(const std::string &context, const std::string &funcID) {
	if (it->first == "opeq") {
		nextToken();

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, ""};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "EQ", funcID, E4Result.second, "L" + l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", "L" + l);

		return {true, s};

	} else if (it->first == "opne") {
		nextToken();

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, ""};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "NE", funcID, E4Result.second, "L" + l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", "L" + l);

		return {true, s};
	} else if (it->first == "ople") {
		nextToken();

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, ""};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "LE", funcID, E4Result.second, "L" + l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", "L" + l);

		return {true, s};
	} else if (it->first == "opgt") {
		nextToken();

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, ""};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "GT", funcID, E4Result.second, "L" + l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", "L" + l);

		return {true, s};
	} else if (it->first == "opge") {
		nextToken();

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, ""};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "GE", funcID, E4Result.second, "L" + l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", "L" + l);

		return {true, s};
	} else if (it->first == "oplt") {
		nextToken();

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, ""};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "LT", funcID, E4Result.second, "L" + l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", "L" + l);

		return {true, s};
	}

	return {true, funcID};
}

FT LL::Expr4(const std::string &context) {
	auto E3Result = Expr3(context);
	if (!E3Result.first) return {false, ""};

	auto E4ListResult = Expr4List(context, E3Result.second);
	if (!E4ListResult.first) return {false, ""};

	return {true, E4ListResult.second};
}

FT LL::Expr4List(const std::string &context, const std::string &funcID) {
	if (it->first == "opplus") {
		nextToken();

		auto E3Result = Expr3(context);
		if (!E3Result.first) return {false, ""};

		auto s = alloc(context);
		generateAtom(context, "ADD", funcID, E3Result.second, s);

		auto E4ListResult = Expr4List(context, s);
		if (!E4ListResult.first) return {false, ""};

		return {true, E4ListResult.second};

	} else if (it->first == "opminus") {
		nextToken();

		auto E3Result = Expr3(context);
		if (!E3Result.first) return {false, ""};

		auto s = alloc(context);
		generateAtom(context, "SUB", funcID, E3Result.second, s);

		auto E4ListResult = Expr4List(context, s);
		if (!E4ListResult.first) return {false, ""};

		return {true, E4ListResult.second};
	}

	return {true, funcID};
}

FT LL::Expr3(const std::string &context) {
	auto E2Result = Expr2(context);
	if (!E2Result.first) return {false, ""};

	auto E3ListResult = Expr3List(context, E2Result.second);
	if (!E3ListResult.first) return {false, ""};

	return {true, E3ListResult.second};

}

FT LL::Expr3List(const std::string &context, const std::string &funcID) {
	if (it->first == "opmul") {
		nextToken();

		auto E2Result = Expr2(context);
		if (!E2Result.first) return {false, ""};

		auto s = alloc(context);
		generateAtom(context, "MUL", funcID, E2Result.second, s);

		auto E3ListResult = Expr3List(context, s);
		if (!E3ListResult.first) return {false, ""};

		return {true, E3ListResult.second};
	}

	return {true, funcID};
}

FT LL::Expr2(const std::string &context) {
	if (it->first == "opnot") {
		nextToken();

		auto E1Result = Expr1(context);
		if (!E1Result.first) return {false, ""};

		auto r = alloc(context);
		generateAtom(context, "NOT", E1Result.second, "", r);

		return {true, r};
	} else {

		auto E1Result = Expr1(context);
		if (!E1Result.first) return {false, ""};

		return {true, E1Result.second};
	}
}

FT LL::Expr1(const std::string &context) {
	if (it->first == "opinc") {
		nextToken();

		if (it->first != "id") return {false, ""};

		auto q = checkVar(context, it->second);
		generateAtom(context, "ADD", q, "1", q);

		nextToken();

		return {true, q};
	} else if (it->first == "lpar") {
		nextToken();

		auto EResult = Expr(context);
		if (!EResult.first) return {false, ""};

		if (it->first != "rpar") return {false, ""};
		nextToken();

		return {true, EResult.second};
	} else if (it->first == "num") {
		auto val = it->second;
		nextToken();

		return {true, val};
	} else if (it->first == "char") {
		auto val = it->second;
		nextToken();

		return {true, val};
	} else if (it->first == "id") {
		auto r = it->second;
		nextToken();

		auto E1ListResult = Expr1List(context, r);
		if (!E1ListResult.first) return {false, ""};

		return {true, E1ListResult.second};
	}

	return {false, ""};
}

FT LL::Expr1List(const std::string &context, const std::string &funcID) {
	if (it->first == "lpar") {
		nextToken();

		auto ArgListResult = ArgList(context);
		if (!ArgListResult.first) return {false, ""};

		if (it->first != "rpar") return {false, ""};
		nextToken();

		auto s = checkFunc(funcID, ArgListResult.second);
		auto r = alloc(context);

		generateAtom(context, "CALL", s, "", r);

		return {true, r};
	}

	if (it->first == "opinc") {
		nextToken();

		auto s = checkVar(context, funcID);
		auto r = alloc(context);

		generateAtom(context, "MOV", s, "", r);
		generateAtom(context, "ADD", s, "1", s);

		return {true, r};
	}

	auto q = checkVar(context, funcID);
	return {true, q};
}

FT LL::ArgList(const std::string &context) {
	if (it->first == "lpar" || it->first == "opinc" || it->first == "num" || it->first == "char" || it->first == "id") {
		auto ERes = Expr(context);
		if (!ERes.first) return {false, ""};

		FT ArgListListResult = ArgListList(context);
		if (!ArgListListResult.first) return {false, ""};

		generateAtom(context, "PARAM", "", "", ERes.second);

		return {true, std::to_string(stoi(ArgListListResult.second) + 1)};
	}

	return {true, "0"};
}

FT LL::ArgListList(const std::string &context) {
	if (it->first == "comma") {
		nextToken();

		if (it->first == "lpar" || it->first == "opinc" || it->first == "num" || it->first == "char" || it->first == "id") {
			auto ERes = Expr(context);
			if (!ERes.first) return {false, ""};

			FT ArgListListResult = ArgListList(context);
			if (!ArgListListResult.first) return {false, ""};

			generateAtom(context, "PARAM", "", "", ERes.second);

			return {true, std::to_string(stoi(ArgListListResult.second) + 1)};
		}
	}

	return {true, "0"};
}