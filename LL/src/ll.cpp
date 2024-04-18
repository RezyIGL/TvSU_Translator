#include "ll.h"
#include <iostream>

LL::LL(std::istream &stream, const std::string &inputPath, const std::string &atomsPath) : lexer {stream} {
	_input = inputPath;
	_atomsInput = atomsPath;
}

void LL::validate() {
	nextToken();
	graphIt = states.begin();

	generateString("StmtList");
	std::string StartContext = "-1";

	if (StmtList(StartContext) && it->first == "eof") {
		myStream.open(_input);
		while (!outputVector.empty()) {
			myStream << outputVector.front() << std::endl;
			outputVector.erase(outputVector.begin());
		}
		myStream.close();

		myStream.open(_atomsInput);
		myStream << "Context: Atom\n" << std::endl;
		while (!atoms.empty()) {
			myStream << atoms.front().context << ": ("
					 << atoms.front().text << ","
					 << atoms.front().first << ","
					 << atoms.front().second << ","
					 << atoms.front().third << ")" << std::endl;
			atoms.erase(atoms.begin());
		}
		myStream.close();

		std::cout << "===========[Translated to Atom Language!]===========";
	} else {
		atoms.clear();
		std::cout << "=============[Your code is incorrect!]=============";
	}
}

void LL::generateAtom(const std::string &context = "", const std::string &text = "", const std::string &first = "", const std::string &second = "",const std::string &third = "") {
	Atom atom = {context, text, first, second, third};
	atoms.emplace_back(atom);
}

std::string LL::newLabel() {
	return "L" + std::to_string(LabelCnt++);
}

std::string LL::alloc(const std::string &scope) {
	return "T" + std::to_string(NewVarCnt++);
}

std::string LL::addVar(const std::string &name, const std::string &scope, const std::string &type, const std::string &init = "") {
	if (AtomicMap.count(scope)) {
		for (const auto &i : AtomicMap[scope]) {
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
	for (const auto &i : AtomicMap["-1"]) {
		if (i.name == name) {
			return "ERROR";
		}
	}

	VarOrFunc temp = {name, "-1", type, length.empty() ? "0" : length, "func", AtomicMapCnt++};
	AtomicMap["-1"].emplace_back(temp);

	return std::to_string(temp.cnt);
}

std::string LL::checkVar(const std::string &scope, const std::string &name) {
	for (const auto &i : AtomicMap[scope]) {
		if (i.name == name && i.kind == "var") return "'" + std::to_string(i.cnt) + "'";
		else if (i.name == name && i.type != "var") return "ERROR";
	}

	for (const auto &i : AtomicMap["-1"]) {
		if (i.name == name && i.kind == "var") return "'" + std::to_string(i.cnt) + "'";
		else if (i.name == name && i.type != "var") return "ERROR";
	}

	return "ERROR";
}

std::string LL::checkFunc(const std::string &name, const std::string &len) {
	for (const auto &i : AtomicMap["-1"]) {
		if (i.name == name && i.kind == "func" && i.init == len) return "'" + std::to_string(i.cnt) + "'";
		else if (i.name == name && i.type != "func") return "ERROR";
	}

	return "ERROR";
}

void LL::nextGraphState(const int &a) {
	states.emplace_back(a);
	graphIt = states.end() - 1;
}

void LL::nextToken() {
	if (myQueue.empty() ||
	    it == myQueue.end() - 1)
	{
		myQueue.emplace_back(lexer.getNextToken());
		it = myQueue.end() - 1;
		return;
	}

	it++;
}

void LL::rollBackChanges(std::vector<Token>::iterator iter) {
	it = iter;
}

bool LL::StmtList(const std::string &context) {
	auto tempGraph = graphIt;

	if (it->first == "eof") return true;

	auto tempIt = it;
	tempGraph = graphIt;

	nextGraphState(1);
	generateString("Stmt");

	if (Stmt(context)) {

		tempGraph = graphIt;

		nextGraphState(0);
		generateString("StmtList");

		if (!StmtList(context)) return false;
	} else {
		outputVector.pop_back();
		outputVector.pop_back();
		rollBackChanges(tempIt);
		rollbackIter();
	}

	rollbackIter();
	return true;
}

bool LL::Stmt(const std::string &context) {
	if (it->first == "eof") return false;

	auto tempIt = it;
	int tempCnt = outVecCnt;

	if (DeclareStmt(context)) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
	}

	tempCnt = outVecCnt;

	if (it->first == "semicolon") {
		nextToken();

		nextGraphState(0);
		generateString("semicolon");

		rollbackIter();
		rollbackIter();
		return true;
	}

	if (context == "-1") return false;

	if (AssignOrCallOp(context)) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
	}

	tempCnt = outVecCnt;

	if (WhileOp(context)) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
		rollbackIter();
	}

	tempCnt = outVecCnt;

	if (ForOp(context)) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
	}

	tempCnt = outVecCnt;

	if (IfOp(context)) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
	}

	tempCnt = outVecCnt;

	if (SwitchOp(context)) {
		return true;
	} else {
//		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
	}

	tempCnt = outVecCnt;

	if (InOp(context)) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
	}

	tempCnt = outVecCnt;

	if (OutOp(context)) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
	}

	tempCnt = outVecCnt;

	if (it->first == "lbrace") {
		nextToken();

		nextGraphState(1);
		generateString("lbrace StmtList");

		tempCnt = outVecCnt;

		if (!StmtList(context)) return false;

		if (it->first != "rbrace") return false;

		nextGraphState(0);
		generateString("rbrace");

		nextToken();
		rollbackIter();
		rollbackIter();
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

		nextGraphState(0);
		generateString("semicolon");

		rollbackIter();

		nextToken();

		rollbackIter();
		return true;
	}

	return false;
}

bool LL::SwitchOp(const std::string &context) {

	nextGraphState(0);
	generateString("SwitchOp");

	if (it->first != "kwswitch") return false;
	nextToken();

	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("kwswitch lpar E");

	if (!Expr(context).first) return false;

	if (it->first != "rpar") return false;
	nextToken();

	if (it->first != "lbrace") return false;
	nextToken();

	nextGraphState(1);
	generateString("rpar lbrace Cases");

	if (!Cases(context)) return false;

	if (it->first != "rbrace") return false;
	nextToken();

	nextGraphState(0);
	generateString("rbrace");

	rollbackIter();
	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::Cases(const std::string &context) {

	nextGraphState(1);
	generateString("ACase");

	if (!ACase(context)) return false;
	rollbackIter();

	nextGraphState(0);
	generateString("CasesList");

	if (!CasesList(context)) return false;

	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::CasesList(const std::string &context) {
	auto tempIt = it;
	int tempCnt = outVecCnt;

	nextGraphState(1);
	generateString("ACase");

	if (ACase(context)) {
		rollbackIter();

		nextGraphState(0);
		generateString("CasesList");

		if (!CasesList(context)) return false;

		rollbackIter();
		return true;
	} else {
		for (int i = 0; i < outVecCnt - tempCnt; i++) {
			rollbackIter();
		}

		outputVector.erase(outputVector.end() - 1, outputVector.end());

		rollBackChanges(tempIt);
		return true;
	}
}

bool LL::ACase(const std::string &context) {

	if (it->first == "kwcase") {

		nextToken();

		if (it->first != "num") return false;
		auto temp = it->second;
		nextToken();

		if (it->first != "colon") return false;

		nextGraphState(0);
		generateString("kwcase " + temp + " colon StmtList");
		nextToken();

		if (!StmtList(context)) return false;

		return true;
	}

	if (it->first == "kwdefault") {
		nextToken();

		if (it->first != "colon") return false;

		nextGraphState(0);
		generateString("kwdefault colon StmtList");
		nextToken();

		if (!StmtList(context)) return false;

		return true;
	}

	return false;
}

bool LL::ForOp(const std::string &context) {
	nextGraphState(0);
	generateString("ForOp");

	if (it->first != "kwfor") return false;

	nextToken();

	if (it->first != "lpar") return false;

	nextGraphState(1);
	generateString("kwfor lpar ForInit");
	nextToken();

	ForInit(context);

	if (it->first != "semicolon") return false;

	nextGraphState(1);
	generateString("semicolon ForExp");
	nextToken();

	ForExp(context);

	if (it->first != "semicolon") return false;

	nextGraphState(1);
	generateString("semicolon ForLoop");
	nextToken();

	nextGraphState(0);
	if (!ForLoop(context)) return false;
	rollbackIter();

	if (it->first != "rpar") return false;

	nextGraphState(0);
	generateString("rpar Stmt");
	nextToken();

	int tempCnt = outVecCnt;

	if (!Stmt(context)) {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		return false;
	}

	rollbackIter();
	rollbackIter();
	return true;
}

void LL::ForInit(const std::string &context) {
	auto tempIt = it;
	int tempCnt = outVecCnt;

	nextGraphState(0);
	generateString("AssignOrCall");

	if (AssignOrCall(context)) {
		rollbackIter();
		return;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		rollBackChanges(tempIt);
		rollbackIter();
		rollbackIter();
	}
}

void LL::ForExp(const std::string &context) {
	auto tempIt = it;
	int tempCnt = outVecCnt;

	nextGraphState(0);
	generateString("E");

	if (Expr(context).first) {
		rollbackIter();
		return;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		for (int i = 0; i < outVecCnt - tempCnt; i++) {
			rollbackIter();
		}
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
	}
}

bool LL::ForLoop(const std::string &context) {
	if (it->first == "opinc") {
		nextToken();

		if (it->first != "id") return false;

		nextGraphState(0);
		generateString("opinc " + it->second);
		rollbackIter();
		nextToken();

		rollbackIter();
		return true;
	}

	auto tempIt = it;

	if (AssignOrCall(context)) {
		rollbackIter();
		return true;
	} else {
		rollBackChanges(tempIt);
	}

	rollbackIter();
	return true;
}

bool LL::IfOp(const std::string &context) {

	nextGraphState(0);
	generateString("IfOp");

	if (it->first != "kwif") return false;
	nextToken();

	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("kwif lpar E");

	if (!Expr(context).first) return false;

	if (it->first != "rpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("rpar Stmt");

	if (!Stmt(context)) return false;

	int tempCnt = outVecCnt;

	if (!ElsePart(context)) {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		return false;
	}

	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::ElsePart(const std::string &context) {

	nextGraphState(0);
	generateString("ElsePart");

	if (it->first == "kwelse") {
		nextToken();

		nextGraphState(0);
		generateString("kwelse Stmt");

		if (!Stmt(context)) return false;
	}

	rollbackIter();
	return true;
}

bool LL::AssignOrCallOp(const std::string &context) {

	nextGraphState(0);
	generateString("AssignOrCallOp");

	nextGraphState(1);
	generateString("AssignOrCall");

	if (!AssignOrCall(context)) return false;

	if (it->first != "semicolon") return false;

	nextGraphState(0);
	generateString("semicolon");
	rollbackIter();
	nextToken();

	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::AssignOrCall(const std::string &context) {
	if (it->first != "id") return false;

	nextGraphState(0);
	generateString(" " + it->second + " AssignOrCall'");

	auto temp = it->second;
	nextToken();

	if (!AssignOrCallList(context, temp)) return false;

	rollbackIter();
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

		rollbackIter();
		return true;
	}

	if (it->first == "lpar") {
		nextToken();

		nextGraphState(1);
		generateString("lpar ArgList");

		auto ArgDADA = ArgList(context);
		if (!ArgDADA.first) return false;
		if (it->first != "rpar") return false;

		nextGraphState(0);
		generateString("rpar");
		rollbackIter();

		auto q = checkFunc(name, ArgDADA.second);
		auto r = alloc(context);
		generateAtom(context, "CALL", q, "", r);

		nextToken();
		rollbackIter();
		return true;
	}
	return false;
}

bool LL::WhileOp(const std::string &context) {

	nextGraphState(0);
	generateString("WhileOp");

	if (it->first != "kwwhile") return false;
	nextToken();

	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("kwwhile lpar E");

	if (!Expr(context).first) return false;

	if (it->first != "rpar") return false;
	nextToken();

	nextGraphState(0);
	generateString("rpar Stmt");

	if (!Stmt(context)) return false;

	rollbackIter();
	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::InOp(const std::string &context) {
	auto tempGraph = graphIt;

	if (it->first != "kwin") return false;

	nextGraphState(0);
	generateString("InOp");
	nextToken();

	tempGraph = graphIt;

	if (it->first != "id") return false;

	auto temp = it->second;

	nextToken();

	tempGraph = graphIt;

	if (it->first != "semicolon") return false;

	nextGraphState(0);
	generateString("kwin " + temp + " semicolon");
	rollbackIter();

	nextToken();

	auto p = checkVar(context, temp);
	generateAtom(context, "IN", "", "", p);

	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::OutOp(const std::string &context) {
	auto tempGraph = graphIt;

	if (it->first != "kwout") return false;

	nextToken();

	nextGraphState(0);
	generateString("OutOp");

	nextGraphState(1);
	generateString("kwout OutOp'");

	if (!OutOpList(context)) return false;
	if (it->first != "semicolon") return false;

	nextToken();

	nextGraphState(0);
	generateString("semicolon");

	rollbackIter();
	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::OutOpList(const std::string &context) {
	if (it->first == "str") {

		nextGraphState(0);
		generateString(it->second);
		rollbackIter();

		auto temp = it->second;
		nextToken();

		generateAtom(context, "OUT", "", "", temp);

		rollbackIter();
		return true;
	} else {
		nextGraphState(0);
		generateString("E");

		auto ERes = Expr(context);
		if (!ERes.first) return false;
		rollbackIter();

		generateAtom(context, "OUT", "", "", ERes.second);
		return true;
	}
}

bool LL::DeclareStmt(const std::string &context) {
	if (it->first == "eof") return false;

	nextGraphState(0);
	generateString("DeclareStmt");

	nextGraphState(1);
	generateString("Type");

	auto TypeResult = Type(context);
	if (!TypeResult.first) return false;

	rollbackIter();

	if (it->first != "id") return false;

	nextGraphState(0);
	generateString(" " + it->second + " DeclareStmt'");

	auto temp = it->second;

	nextToken();

	if (!DeclareStmtList(context, TypeResult.second, temp)) return false;

	rollbackIter();
	rollbackIter();
	return true;
}

FT LL::Type(const std::string &context) {
	if (it->first == "eof") return {false, ""};

	if (it->first == "kwint" ||
	    it->first == "kwchar")
	{
		nextGraphState(0);
		generateString(it->first);

		rollbackIter();

		auto temp = it->first;

		nextToken();
		return {true, temp};
	}

	rollbackIter();
	return {false, ""};
}

bool LL::DeclareStmtList(const std::string &context, const std::string &type, const std::string &name) {
	if (it->first == "eof") return false;

	if (it->first == "lpar") {
		if (stoi(context) > -1) return false;

		nextToken();

		nextGraphState(1);
		generateString("lpar ParamList");

		auto Pararam = ParamList(context);
		if (!Pararam.first) return false;

		if (it->first != "rpar") return false;

		nextToken();

		auto TC = addFunc(name, type, Pararam.second);

		if (it->first != "lbrace") return false;

		nextGraphState(1);
		generateString("rpar lbrace StmtList");
		nextToken();

		if (!StmtList(TC)) return false;

		if (it->first != "rbrace") return false;

		nextGraphState(0);
		generateString("rbrace");
		rollbackIter();
		nextToken();

		// TODO: Reassembly {RET,,,0}. Make it optional
		// generateAtom(context, "RET", "", "", "0");

		rollbackIter();
		return true;
	} else if (it->first == "opassign") {
		nextToken();

		if (it->first == "num") {
			nextGraphState(1);
			generateString("opassign " + it->second + " DeclareVarList");

			addVar(name, context, type, it->second);

			nextToken();

			auto DecVarListRes = DeclareVarList(context, type);
			if (!DecVarListRes) return false;

			if (it->first != "semicolon") return false;

			nextGraphState(0);
			generateString("semicolon");
			nextToken();

			rollbackIter();
			rollbackIter();
			return true;
		} else if (it->first == "char") {
			nextGraphState(1);
			generateString("opassign " + it->second + " DeclareVarList");

			addVar(name, context, type, it->second);

			nextToken();

			auto DecVarListRes = DeclareVarList(context, type);
			if (!DecVarListRes) return false;

			if (it->first != "semicolon") return false;

			nextGraphState(0);
			generateString("semicolon");
			nextToken();

			rollbackIter();
			rollbackIter();
			return true;
		} else {
			return false;
		}
	} else {

		nextGraphState(1);
		generateString("DeclareVarList");

		addVar(name, context, type);

		if (!DeclareVarList(context, type)) return false;
		if (it->first != "semicolon") return false;

		nextGraphState(0);
		generateString("semicolon");
		rollbackIter();
		nextToken();

		rollbackIter();
		return true;
	}
}

bool LL::DeclareVarList(const std::string &context, const std::string &type) {
	if (it->first == "eof") return false;

	if (it->first == "comma") {

		nextToken();

		if (it->first != "id") return false;

		nextGraphState(1);
		generateString("comma " + it->second + " InitVar");
		auto temp = it->second;
		nextToken();

		if (!InitVar(context, type, temp)) return false;

		rollbackIter();
		nextGraphState(0);
		generateString("DeclareVarList");

		if (!DeclareVarList(context, type)) return false;
	}

	rollbackIter();
	return true;
}

bool LL::InitVar(const std::string &context, const std::string &r, const std::string &s) {
	if (it->first == "eof") return false;

	if (it->first == "opassign") {
		nextToken();

		if (it->first == "num" ||
		    it->first == "char")
		{
			nextGraphState(0);
			generateString("opassign " + it->second);
			rollbackIter();

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

	auto tempIt = it;
	auto tempCnt = outVecCnt;

	nextGraphState(1);
	generateString("Type");

	auto TypeResult = Type(context);
	if (TypeResult.first) {
		if (it->first != "id") return {false, ""};

		rollbackIter();

		addVar(it->second, context, TypeResult.second);

		nextGraphState(0);
		generateString(" " + it->second + " ParamList'");
		nextToken();

		auto da = ParamListList(context);
		if (!da.first) return {false, ""};

		rollbackIter();
		return {true, std::to_string(stoi(da.second) + 1)};
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		rollBackChanges(tempIt);
	}

	rollbackIter();
	return {true, "0"};
}

FT LL::ParamListList(const std::string &context) {
	if (it->first == "eof") return {false, ""};

	if (it->first == "comma") {
		nextToken();

		nextGraphState(1);
		generateString("comma Type");

		auto TypeResult = Type(context);
		if (!TypeResult.first) return {false, ""};

		rollbackIter();

		if (it->first != "id") return {false, ""};

		addVar(it->second, context, TypeResult.second);

		nextGraphState(0);
		generateString(" " + it->second + " ParamList'");
		nextToken();

		auto net = ParamListList(context);
		if (!net.first) return {false, ""};

		rollbackIter();
		return {true, std::to_string(stoi(net.second) + 1)};
	}

	rollbackIter();
	return {true, "0"};
}

void LL::generateString(const std::string &abiba) {
	std::string aboba;
	for (auto i = states.begin(); i != states.end(); ++i) {
		if (i == states.end() - 1) {
			if (*i == 1) {
				aboba += "├";
			} else {
				aboba += "└";
			}

			break;
		}

		if (*i == 1) {
			aboba += "│ ";
		} else {
			aboba += " ";
		}
	}

	outVecCnt++;
	outputVector.push_back(aboba + abiba);
}

void LL::rollbackIter() {
	states.pop_back();
	graphIt = states.end() - 1;
}

FT LL::Expr(const std::string &context) {
	auto tempGraph = graphIt;

	nextGraphState(0);
	generateString("E7");

	auto E7Result = Expr7(context);
	if (!E7Result.first) return {false, "-2"};

	rollbackIter();
	return {true, E7Result.second};
}

FT LL::Expr7(const std::string &context) {
	auto tempGraph = graphIt;

	nextGraphState(1);
	generateString("E6");

	auto E6Result = Expr6(context);
	if (!E6Result.first) return {false, "-2"};

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E7'");

	auto E7ListResult = Expr7List(context, E6Result.second);
	if (!E7ListResult.first) return {false, "-2"};

	rollbackIter();
	return {true, E7ListResult.second};
}

FT LL::Expr7List(const std::string &context, const std::string &funcID) {
	if (it->first == "opor") {
		nextToken();
		nextGraphState(1);
		generateString("opor E6");

		auto tempGraph = graphIt;

		auto E6Result = Expr6(context);
		if (!E6Result.first) return {false, "-2"};

		tempGraph = graphIt;

		auto s = alloc(context);
		generateAtom(context, "OR", funcID, E6Result.second, s);

		nextGraphState(0);
		generateString("E7'");
		
		auto E7ListResult = Expr7List(context, s);
		if (!E7ListResult.first) return {false, "-2"};

		rollbackIter();
		return {true, E7ListResult.second};
	}

	rollbackIter();
	return {true, funcID};
}

FT LL::Expr6(const std::string &context) {
	auto tempGraph = graphIt;

	nextGraphState(1);
	generateString("E5");

	auto E5Result = Expr5(context);
	if (!E5Result.first) return {false, "-2"};

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E6'");

	auto E6ListResult = Expr6List(context, E5Result.second);
	if (!E6ListResult.first) return {false, "-2"};

	rollbackIter();
	return {true, E6ListResult.second};
}

FT LL::Expr6List(const std::string &context, const std::string &funcID) {
	if (it->first == "opand") {
		nextToken();
		nextGraphState(1);
		generateString("opand E5");

		auto tempGraph = graphIt;

		auto E5Result = Expr5(context);
		if (!E5Result.first) return {false, "-2"};

		tempGraph = graphIt;

		auto s = alloc(context);
		generateAtom(context, "AND", funcID, E5Result.second, s);

		nextGraphState(0);
		generateString("E6'");

		auto E6ListResult = Expr6List(context, s);

		if (!E6ListResult.first) {
			
			return {false, "-2"};
		}

		rollbackIter();
		return {true, E6ListResult.second};
	}

	rollbackIter();
	return {true, funcID};
}

FT LL::Expr5(const std::string &context) {
	auto tempGraph = graphIt;

	nextGraphState(1);
	generateString("E4");

	auto E4Result = Expr4(context);
	if (!E4Result.first) return {false, "-2"};

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E5'");

	auto E5ListResult = Expr5List(context, E4Result.second);

	if (!E5ListResult.first) return {false, "-2"};

	rollbackIter();
	return {true, E5ListResult.second};
}

FT LL::Expr5List(const std::string &context, const std::string &funcID) {
	if (it->first == "opeq") {
		nextToken();
		nextGraphState(0);
		generateString("opeq E4");

		auto tempGraph = graphIt;

		auto E4Result = Expr4(context);
		if (!E4Result.first) {
			
			return {false, "-2"};
		}

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "EQ", funcID, E4Result.second, l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", l);

		rollbackIter();
		return {true, s};

	} else if (it->first == "opne") {
		nextToken();
		nextGraphState(0);
		generateString("opne E4");

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, "-2"};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "NE", funcID, E4Result.second, l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", l);

		rollbackIter();
		return {true, s};
	} else if (it->first == "ople") {
		nextToken();
		nextGraphState(0);
		generateString("ople E4");

		auto tempGraph = graphIt;

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, "-2"};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context, "MOV", "1", "", s);
		generateAtom(context, "LE", funcID, E4Result.second, l);
		generateAtom(context, "MOV", "0", "", s);
		generateAtom(context, "LBL", "", "", l);

		rollbackIter();
		return {true, s};
	} else if (it->first == "opgt") {
		nextToken();
		nextGraphState(0);
		generateString("opgt E4");

		auto tempGraph = graphIt;

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, "-2"};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context,"MOV", "1", "", s);
		generateAtom(context,"GT", funcID, E4Result.second, l);
		generateAtom(context,"MOV", "0", "", s);
		generateAtom(context,"LBL", "", "", l);

		rollbackIter();
		return {true, s};
	} else if (it->first == "opge") {
		nextToken();
		nextGraphState(0);
		generateString("opge E4");

		auto tempGraph = graphIt;

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, "-2"};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context,"MOV", "1", "", s);
		generateAtom(context,"GE", funcID, E4Result.second, l);
		generateAtom(context,"MOV", "0", "", s);
		generateAtom(context,"LBL", "", "", l);

		rollbackIter();
		return {true, s};
	} else if (it->first == "oplt") {
		nextToken();
		nextGraphState(0);
		generateString("oplt E4");

		auto tempGraph = graphIt;

		auto E4Result = Expr4(context);
		if (!E4Result.first) return {false, "-2"};

		auto s = alloc(context);
		auto l = newLabel();

		generateAtom(context,"MOV", "1", "", s);
		generateAtom(context,"LT", funcID, E4Result.second, l);
		generateAtom(context,"MOV", "0", "", s);
		generateAtom(context,"LBL", "", "", l);

		rollbackIter();
		return {true, s};
	}

	rollbackIter();
	return {true, funcID};
}

FT LL::Expr4(const std::string &context) {
	auto tempGraph = graphIt;
	nextGraphState(1);
	generateString("E3");

	auto E3Result = Expr3(context);
	if (!E3Result.first) return {false, "-2"};

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E4'");

	auto E4ListResult = Expr4List(context, E3Result.second);
	if (!E4ListResult.first) return {false, "-2"};

	rollbackIter();
	return {true, E4ListResult.second};
}

FT LL::Expr4List(const std::string &context, const std::string &funcID) {
	if (it->first == "opplus") {
		nextToken();
		nextGraphState(1);
		generateString("opplus E3");

		auto tempGraph = graphIt;

		auto E3Result = Expr3(context);
		if (!E3Result.first) return {false, "-2"};

		tempGraph = graphIt;

		auto s = alloc(context);
		generateAtom(context,"ADD", funcID, E3Result.second, s);

		nextGraphState(0);
		generateString("E4'");
		auto E4ListResult = Expr4List(context, s);
		if (!E4ListResult.first) {
			
			return {false, "-2"};
		}

		rollbackIter();
		return {true, E4ListResult.second};

	} else if (it->first == "opminus") {
		nextToken();
		nextGraphState(1);
		generateString("opminus E3");

		auto tempGraph = graphIt;

		auto E3Result = Expr3(context);
		if (!E3Result.first) return {false, "-2"};

		tempGraph = graphIt;

		auto s = alloc(context);
		generateAtom(context,"SUB", funcID, E3Result.second, s);

		nextGraphState(0);
		generateString("E4'");
		auto E4ListResult = Expr4List(context, s);
		if (!E4ListResult.first) {
			
			return {false, "-2"};
		}

		rollbackIter();
		return {true, E4ListResult.second};
	}

	rollbackIter();
	return {true, funcID};
}

FT LL::Expr3(const std::string &context) {
	auto tempGraph = graphIt;
	nextGraphState(1);
	generateString("E2");

	auto E2Result = Expr2(context);
	if (!E2Result.first) return {false, "-2"};

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E3'");

	auto E3ListResult = Expr3List(context, E2Result.second);
	if (!E3ListResult.first) return {false, "-2"};

	rollbackIter();
	return {true, E3ListResult.second};
}

FT LL::Expr3List(const std::string &context, const std::string &funcID) {
	if (it->first == "opmul") {
		nextToken();
		nextGraphState(1);
		generateString("opmul E2");

		auto tempGraph = graphIt;

		auto E2Result = Expr2(context);
		if (!E2Result.first) return {false, "-2"};

		auto s = alloc(context);
		generateAtom(context,"MUL", funcID, E2Result.second, s);

		nextGraphState(0);
		generateString("E3'");

		auto E3ListResult = Expr3List(context, s);
		if (!E3ListResult.first) {
			
			return {false, "-2"};
		}

		rollbackIter();
		return {true, E3ListResult.second};
	}

	rollbackIter();
	return {true, funcID};
}

FT LL::Expr2(const std::string &context) {
	auto tempGraph = graphIt;

	if (it->first == "opnot") {
		nextToken();
		nextGraphState(0);
		generateString("opnot E1");

		auto E1Result = Expr1(context);
		if (!E1Result.first) return {false, "-2"};

		auto r = alloc(context);
		generateAtom(context,"NOT", E1Result.second, "", r);

		rollbackIter();
		rollbackIter();
		return {true, r};
	} else {
		nextGraphState(0);
		generateString("E1");

		auto E1Result = Expr1(context);
		if (!E1Result.first) return {false, "-2"};

		rollbackIter();
		rollbackIter();
		return {true, E1Result.second};
	}
}

FT LL::Expr1(const std::string &context) {
	auto tempGraph = graphIt;

	if (it->first == "opinc") {
		nextToken();
		nextGraphState(1);
		generateString("opinc");

		rollbackIter();

		nextGraphState(0);
		if (it->first != "id") return {false, "-2"};
		generateString(" " + it->second);
		rollbackIter();

		auto q = checkVar(context, it->second);
		generateAtom(context,"ADD", q, "1", q);

		nextToken();
		return {true, q};
	} else if (it->first == "lpar") {

		nextGraphState(1);
		generateString("lpar E");

		nextToken();

		auto EResult = Expr(context);
		if (!EResult.first) return {false, "-2"};

		nextGraphState(0);
		if (it->first != "rpar") return {false, "-2"};

		generateString("rpar");

		rollbackIter();

		nextToken();
		return {true, EResult.second};
	} else if (it->first == "num") {
		nextGraphState(0);
		generateString(" " + it->second);
		auto val = it->second;
		nextToken();
		rollbackIter();
		return {true, val};
	} else if (it->first == "char") {
		nextGraphState(0);
		generateString(" " + it->second);
		auto val = it->second;
		nextToken();
		rollbackIter();
		return {true, val};
	} else if (it->first == "id") {
		nextGraphState(0);
		generateString(" " + it->second + " E1'");
		auto r = it->second;
		nextToken();

		auto E1ListResult = Expr1List(context, r);
		if (!E1ListResult.first) return {false, "-2"};

		rollbackIter();
		return {true, E1ListResult.second};
	}

	return {false, "-2"};
}

FT LL::Expr1List(const std::string &context, const std::string &funcID) {
	if (it->first == "lpar") {
		nextToken();

		auto tempGraph = graphIt;
		nextGraphState(1);
		generateString("lpar ArgList");

		auto ArgListResult = ArgList(context);
		if (!ArgListResult.first) return {false, "-2"};

		tempGraph = graphIt;

		if (it->first != "rpar") return {false, "-2"};

		nextGraphState(0);
		generateString("rpar");

		nextToken();
		rollbackIter();

		auto s = checkFunc(funcID, ArgListResult.second);
		auto r = alloc(context);

		generateAtom(context, "CALL", s, "", r);

		return {true, r};
	}

	if (it->first == "opinc") {
		nextToken();

		nextGraphState(0);
		generateString("opinc");
		rollbackIter();

		auto s = checkVar(context, funcID);
		auto r = alloc(context);

		generateAtom(context,"MOV", s, "", r);
		generateAtom(context,"ADD", s, "1", s);

		return {true, r};
	}

	auto q = checkVar(context, funcID);
	return {true, q};
}

FT LL::ArgList(const std::string &context) {
	auto tempIt = it;
	auto tempCnt = outVecCnt;

	nextGraphState(1);
	generateString("E");

	auto EResult = Expr(context);
	if (EResult.first) {

		nextGraphState(0);
		generateString("ArgList'");

		auto ArgListListResult = ArgListList(context);
		if (!ArgListListResult.first) return {false, "-2"};

		generateAtom(context, "PARAM", "", "", EResult.second);

		rollbackIter();
		return {true, std::to_string(stoi(ArgListListResult.second) + 1)};
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		for (int i = 0; i < outVecCnt - tempCnt; i++) {
			rollbackIter();
		}
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
		return {true, "0"};
	}
}

FT LL::ArgListList(const std::string &context) {
	if (it->first == "comma") {
		auto tempGraph = graphIt;

		nextToken();

		auto tempIt = it;
		auto tempCnt = outVecCnt;

		nextGraphState(1);
		generateString("E");

		auto EResult = Expr(context);
		if (EResult.first) {

			nextGraphState(0);
			generateString("ArgList'");

			auto ArgListListResult = ArgListList(context);
			if (!ArgListListResult.first) return {false, "-2"};

			generateAtom(context, "PARAM", "", "", EResult.second);

			rollbackIter();
			return {true, std::to_string(stoi(ArgListListResult.second) + 1)};
		} else {
			outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
			outVecCnt = tempCnt;
			rollBackChanges(tempIt);
		}
	}

	rollbackIter();
	return {true, "0"};
}