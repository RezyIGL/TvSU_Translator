#include "ll.h"
#include <iostream>

LL::LL(std::istream &stream, const std::string &inputPath, const std::string &atomsPath) : lexer {stream} {
	_input = inputPath;
	_atomsInput = atomsPath;
}

void LL::validate() {
	nextToken();
	graphIt = states.begin();

	// TODO: Return StmtList as entry point.

	generateString("StmtList");
	std::string context = "-1";
	if (Expr(context).first && it->first == "eof") {
		std::cout << "Accepted!" << std::endl;
	} else {
		atoms.clear();
		std::cout << "Incorrect Expression!" << std::endl;
	}

	myStream.open(_input);
	while (!outputVector.empty()) {
		myStream << outputVector.front() << std::endl;
		outputVector.erase(outputVector.begin());
	}
	myStream.close();

	myStream.open(_atomsInput);
	while (!atoms.empty()) {
		myStream << atoms.front().context << ": (" << atoms.front().text << "," << atoms.front().first << "," << atoms.front().second << "," << atoms.front().third << ")" << std::endl;
		atoms.erase(atoms.begin());
	}
	myStream.close();
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
// TODO: addVar
std::string LL::addVar(const std::string &name, const std::string &scope, const std::string &type, const std::string &init = "0") {return "newVar";}
// TODO: addFunc
std::string LL::addFunc(const std::string &name, const std::string &type) {return "newFunc";}
// TODO: checkVar
std::string LL::checkVar(const std::string &scope, const std::string &name) {return "isVar";}
// TODO: checkFunc
std::string LL::checkFunc(const std::string &name, const std::string &len) {return "isFunc";}

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

bool LL::StmtList() {
	auto tempGraph = graphIt;

	if (it->first == "eof") return true;

	auto tempIt = it;
	tempGraph = graphIt;

	nextGraphState(1);
	generateString("Stmt");

	if (Stmt()) {

		tempGraph = graphIt;

		nextGraphState(0);
		generateString("StmtList");

		if (!StmtList()) return false;
	} else {
		outputVector.erase(outputVector.end() - 1, outputVector.end());
		rollBackChanges(tempIt);
		rollbackIter();
	}

	rollbackIter();
	return true;
}

bool LL::Stmt() {
	if (it->first == "eof") return false;

	auto tempIt = it;
	int tempCnt = outVecCnt;

	if (DeclareStmt()) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
	}

	tempCnt = outVecCnt;

	if (AssignOrCallOp()) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
	}

	tempCnt = outVecCnt;

	if (WhileOp()) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
		rollbackIter();
	}

	tempCnt = outVecCnt;

	if (ForOp()) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
	}

	tempCnt = outVecCnt;

	if (IfOp()) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
	}

	tempCnt = outVecCnt;

	if (SwitchOp()) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
	}

	tempCnt = outVecCnt;

	if (InOp()) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
	}

	tempCnt = outVecCnt;

	if (OutOp()) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
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

	if (it->first == "lbrace") {
		nextToken();

		nextGraphState(1);
		generateString("lbrace StmtList");

		tempCnt = outVecCnt;

		if (!StmtList()) return false;

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

		// TODO: Change context to auto input
		if (!Expr("-1").first) return false;
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

bool LL::SwitchOp() {

	nextGraphState(0);
	generateString("SwitchOp");

	if (it->first != "kwswitch") return false;
	nextToken();

	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("kwswitch lpar E");

	// TODO: Change context to auto input
	if (!Expr("-1").first) return false;

	if (it->first != "rpar") return false;
	nextToken();

	if (it->first != "lbrace") return false;
	nextToken();

	nextGraphState(1);
	generateString("rpar lbrace Cases");

	if (!Cases()) return false;

	if (it->first != "rbrace") return false;
	nextToken();

	nextGraphState(0);
	generateString("rbrace");

	rollbackIter();
	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::Cases() {

	nextGraphState(1);
	generateString("ACase");

	if (!ACase()) return false;
	rollbackIter();

	nextGraphState(0);
	generateString("CasesList");

	if (!CasesList()) return false;

	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::CasesList() {
	auto tempIt = it;
	int tempCnt = outVecCnt;

	nextGraphState(1);
	generateString("ACase");

	if (ACase()) {
		rollbackIter();

		nextGraphState(0);
		generateString("CasesList");

		if (!CasesList()) return false;

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

bool LL::ACase() {

	if (it->first == "kwcase") {

		nextToken();

		if (it->first != "num") return false;
		auto temp = it->second;
		nextToken();

		if (it->first != "colon") return false;

		nextGraphState(0);
		generateString("kwcase " + temp + " colon StmtList");
		nextToken();

		if (!StmtList()) return false;

		return true;
	}

	if (it->first == "kwdefault") {
		nextToken();

		if (it->first != "colon") return false;

		nextGraphState(0);
		generateString("kwdefault colon StmtList");
		nextToken();

		if (!StmtList()) return false;

		return true;
	}

	return false;
}

bool LL::ForOp() {
	nextGraphState(0);
	generateString("ForOp");

	if (it->first != "kwfor") return false;

	nextToken();

	if (it->first != "lpar") return false;

	nextGraphState(1);
	generateString("kwfor lpar ForInit");
	nextToken();

	ForInit();

	if (it->first != "semicolon") return false;

	nextGraphState(1);
	generateString("semicolon ForExp");
	nextToken();

	ForExp();

	if (it->first != "semicolon") return false;

	nextGraphState(1);
	generateString("semicolon ForLoop");
	nextToken();

	nextGraphState(0);
	if (!ForLoop()) return false;
	rollbackIter();

	if (it->first != "rpar") return false;

	nextGraphState(0);
	generateString("rpar Stmt");
	nextToken();

	int tempCnt = outVecCnt;

	if (!Stmt()) {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		return false;
	}

	rollbackIter();
	rollbackIter();
	return true;
}

void LL::ForInit() {
	auto tempIt = it;
	int tempCnt = outVecCnt;

	nextGraphState(0);
	generateString("AssignOrCall");

	if (AssignOrCall()) {
		rollbackIter();
		return;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		rollBackChanges(tempIt);
		rollbackIter();
		rollbackIter();
	}
}

void LL::ForExp() {
	auto tempIt = it;
	int tempCnt = outVecCnt;

	nextGraphState(0);
	generateString("E");

	// TODO: Change context to auto input
	if (Expr("-1").first) {
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

bool LL::ForLoop() {
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

	if (AssignOrCall()) {
		rollbackIter();
		return true;
	} else {
		rollBackChanges(tempIt);
	}

	rollbackIter();
	return true;
}

bool LL::IfOp() {

	nextGraphState(0);
	generateString("IfOp");

	if (it->first != "kwif") return false;
	nextToken();

	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("kwif lpar E");

	// TODO: Change context to auto input
	if (!Expr("-1").first) return false;

	if (it->first != "rpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("rpar Stmt");

	if (!Stmt()) return false;

	int tempCnt = outVecCnt;

	if (!ElsePart()) {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		return false;
	}

	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::ElsePart() {

	nextGraphState(0);
	generateString("ElsePart");

	if (it->first == "kwelse") {
		nextToken();

		nextGraphState(0);
		generateString("kwelse Stmt");

		if (!Stmt()) return false;
	}

	rollbackIter();
	return true;
}

bool LL::AssignOrCallOp() {

	nextGraphState(0);
	generateString("AssignOrCallOp");

	nextGraphState(1);
	generateString("AssignOrCall");

	if (!AssignOrCall()) return false;

	if (it->first != "semicolon") return false;

	nextGraphState(0);
	generateString("semicolon");
	rollbackIter();
	nextToken();

	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::AssignOrCall() {
	if (it->first != "id") return false;

	nextGraphState(0);
	generateString(" " + it->second + " AssignOrCall'");

	nextToken();
	if (!AssignOrCallList()) return false;

	rollbackIter();
	return true;
}

bool LL::AssignOrCallList() {
	if (it->first == "opassign") {
		nextToken();

		nextGraphState(0);
		generateString("opassign E");

		// TODO: Change context to auto input
		if (!Expr("-1").first) return false;

		rollbackIter();
		return true;
	}

	if (it->first == "lpar") {
		nextToken();

		nextGraphState(1);
		generateString("lpar ArgList");

		// TODO: Change context to auto input
		if (!ArgList("-1").first) return false;
		if (it->first != "rpar") return false;

		nextGraphState(0);
		generateString("rpar");
		rollbackIter();

		nextToken();
		rollbackIter();
		return true;
	}
	return false;
}

bool LL::WhileOp() {

	nextGraphState(0);
	generateString("WhileOp");

	if (it->first != "kwwhile") return false;
	nextToken();

	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("kwwhile lpar E");

	// TODO: Change context to auto input
	if (!Expr("-1").first) return false;

	if (it->first != "rpar") return false;
	nextToken();

	nextGraphState(0);
	generateString("rpar Stmt");

	if (!Stmt()) return false;

	rollbackIter();
	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::InOp() {
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

	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::OutOp() {
	auto tempGraph = graphIt;

	if (it->first != "kwout") return false;

	nextToken();

	nextGraphState(0);
	generateString("OutOp");

	nextGraphState(1);
	generateString("kwout E");

	// TODO: Change context to auto input
	if (!Expr("-1").first) return false;
	if (it->first != "semicolon") return false;

	nextToken();

	nextGraphState(0);
	generateString("semicolon");

	rollbackIter();
	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::DeclareStmt() {
	if (it->first == "eof") return false;

	nextGraphState(0);
	generateString("DeclareStmt");

	nextGraphState(1);
	generateString("Type");

	if (!Type()) return false;

	rollbackIter();

	if (it->first != "id") return false;

	nextGraphState(0);
	generateString(" " + it->second + " DeclareStmt'");
	nextToken();

	if (!DeclareStmtList()) return false;

	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::Type() {
	if (it->first == "eof") return false;

	if (it->first == "kwint" ||
	    it->first == "kwchar")
	{
		nextGraphState(0);
		generateString(it->first);

		rollbackIter();

		nextToken();
		return true;
	}

	rollbackIter();
	return false;
}

bool LL::DeclareStmtList() {
	if (it->first == "eof") return false;

	if (it->first == "lpar") {
		nextToken();

		nextGraphState(1);
		generateString("lpar ParamList");

		if (!ParamList()) return false;

		if (it->first != "rpar") return false;

		nextToken();

		if (it->first != "lbrace") return false;

		nextGraphState(1);
		generateString("rpar lbrace StmtList");
		nextToken();

		if (!StmtList()) return false;

		if (it->first != "rbrace") return false;

		nextGraphState(0);
		generateString("rbrace");
		rollbackIter();
		nextToken();

		rollbackIter();
		return true;
	} else if (it->first == "opassign") {
		nextToken();

		if (it->first == "num") {
			nextGraphState(1);
			generateString("opassign " + it->second + " DeclareVarList");
			nextToken();

			if (!DeclareVarList()) return false;

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
			nextToken();

			if (!DeclareVarList()) return false;

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

		if (!DeclareVarList()) return false;
		if (it->first != "semicolon") return false;

		nextGraphState(0);
		generateString("semicolon");
		rollbackIter();
		nextToken();

		rollbackIter();
		return true;
	}
}

bool LL::DeclareVarList() {
	if (it->first == "eof") return false;

	if (it->first == "comma") {

		nextToken();

		if (it->first != "id") return false;

		nextGraphState(1);
		generateString("comma " + it->second + " InitVar");
		nextToken();

		if (!InitVar()) return false;

		rollbackIter();
		nextGraphState(0);
		generateString("DeclareVarList");

		if (!DeclareVarList()) return false;
	}

	rollbackIter();
	return true;
}

bool LL::InitVar() {
	if (it->first == "eof") return false;

	if (it->first == "opassign") {
		nextToken();

		if (it->first == "num" ||
		    it->first == "char")
		{
			nextGraphState(0);
			generateString("opassign " + it->second);
			rollbackIter();
			nextToken();
			return true;
		}
		return false;
	}

	return true;
}

bool LL::ParamList() {
	if (it->first == "eof") return false;

	auto tempIt = it;
	auto tempCnt = outVecCnt;

	nextGraphState(1);
	generateString("Type");

	if (Type()) {
		if (it->first != "id") return false;

		rollbackIter();

		nextGraphState(0);
		generateString(" " + it->second + " ParamList'");
		nextToken();

		if (!ParamListList()) return false;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		rollBackChanges(tempIt);
	}

	rollbackIter();
	return true;
}

bool LL::ParamListList() {
	if (it->first == "eof") return false;

	if (it->first == "comma") {
		nextToken();

		nextGraphState(1);
		generateString("comma Type");

		if (!Type()) return false;

		rollbackIter();

		if (it->first != "id") return false;

		nextGraphState(0);
		generateString(" " + it->second + " ParamList'");
		nextToken();

		if (!ParamListList()) return false;
	}

	rollbackIter();
	return true;
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
		if (!E7ListResult.first) {
			
			return {false, "-2"};
		}

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

	auto s = alloc(context);
	auto E5ListResult = Expr5List(context, s);

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

	return {true, funcID};
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