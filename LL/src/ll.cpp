#include "ll.h"
#include <iostream>

LL::LL(std::istream &stream, const std::string &inputPath) : lexer {stream} {
	_input = inputPath;
}

void LL::validate() {
	nextToken();
	graphIt = states.begin();

	generateString("StmtList");
	if (StmtList() && it->first == "eof") {
		std::cout << "Accepted!" << std::endl;
	} else {
		std::cout << "Incorrect Expression!" << std::endl;
	}

	myStream.open(_input);

	while (!outputVector.empty()) {
		myStream << outputVector.front() << std::endl;
		outputVector.erase(outputVector.begin());
	}

	myStream.close();
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
		outputVector.pop_back();
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

		rollbackIter();

		nextToken();
		rollbackIter();
		return true;
	}

	if (it->first == "kwreturn") {
		nextToken();

		nextGraphState(1);
		generateString("kwreturn E");

		if (!Expr()) return false;
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

	if (!Expr()) return false;

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

	if (Expr()) {
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

	if (!Expr()) return false;

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

		if (!Expr()) return false;

		rollbackIter();
		return true;
	}

	if (it->first == "lpar") {
		nextToken();

		nextGraphState(1);
		generateString("lpar ArgList");

		if (!ArgList()) return false;
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

	if (!Expr()) return false;

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

	if (!Expr()) return false;
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

bool LL::Expr() {
	auto tempGraph = graphIt;

	nextGraphState(0);
	generateString("E7");

	if (!Expr7()) return false;

	rollbackIter();
	return true;
}

bool LL::Expr7() {
	auto tempGraph = graphIt;

	nextGraphState(1);
	generateString("E6");
	if (!Expr6()) return false;

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E7'");
	if (!Expr7List()) return false;

	rollbackIter();
	return true;
}

bool LL::Expr7List() {
	if (it->first == "opor") {
		nextToken();
		nextGraphState(1);
		generateString("opor E6");

		auto tempGraph = graphIt;

		if (!Expr6()) return false;

		tempGraph = graphIt;

		nextGraphState(0);
		generateString("E7'");
		if (!Expr7List()) return false;
	}

	rollbackIter();
	return true;
}

bool LL::Expr6() {
	auto tempGraph = graphIt;

	nextGraphState(1);
	generateString("E5");
	if (!Expr5()) return false;

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E6'");
	if (!Expr6List()) return false;

	rollbackIter();
	return true;
}

bool LL::Expr6List() {
	if (it->first == "opand") {
		nextToken();
		nextGraphState(1);
		generateString("opand E5");

		auto tempGraph = graphIt;

		if (!Expr5()) return false;

		tempGraph = graphIt;

		nextGraphState(0);
		generateString("E6'");
		if (!Expr6List()) return false;
	}

	rollbackIter();
	return true;
}

bool LL::Expr5() {
	auto tempGraph = graphIt;

	nextGraphState(1);
	generateString("E4");
	if (!Expr4()) return false;

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E5'");
	if (!Expr5List()) return false;

	rollbackIter();
	return true;
}

bool LL::Expr5List() {
	if (it->first == "opeq") {
		nextToken();
		nextGraphState(0);
		generateString("opeq E4");

		auto tempGraph = graphIt;

		if (!Expr4()) return false;

	} else if (it->first == "opne") {
		nextToken();
		nextGraphState(0);
		generateString("opne E4");

		auto tempGraph = graphIt;

		if (!Expr4()) return false;

	} else if (it->first == "ople") {
		nextToken();
		nextGraphState(0);
		generateString("ople E4");

		auto tempGraph = graphIt;

		if (!Expr4()) return false;

	} else if (it->first == "opgt") {
		nextToken();
		nextGraphState(0);
		generateString("opgt E4");

		auto tempGraph = graphIt;

		if (!Expr4()) return false;

	} else if (it->first == "opge") {
		nextToken();
		nextGraphState(0);
		generateString("opge E4");

		auto tempGraph = graphIt;

		if (!Expr4()) return false;

	} else if (it->first == "oplt") {
		nextToken();
		nextGraphState(0);
		generateString("oplt E4");

		auto tempGraph = graphIt;

		if (!Expr4()) return false;
	}

	rollbackIter();
	return true;
}

bool LL::Expr4() {
	auto tempGraph = graphIt;
	nextGraphState(1);
	generateString("E3");

	if (!Expr3()) return false;

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E4'");
	if (!Expr4List()) return false;

	rollbackIter();
	return true;
}

bool LL::Expr4List() {
	if (it->first == "opplus") {
		nextToken();
		nextGraphState(1);
		generateString("opplus E3");

		auto tempGraph = graphIt;

		if (!Expr3()) return false;

		tempGraph = graphIt;

		nextGraphState(0);
		generateString("E4'");

		if (!Expr4List()) return false;

	} else if (it->first == "opminus") {
		nextToken();
		nextGraphState(1);
		generateString("opminus E3");

		auto tempGraph = graphIt;

		if (!Expr3()) return false;

		tempGraph = graphIt;

		nextGraphState(0);
		generateString("E4'");

		if (!Expr4List()) return false;

	}

	rollbackIter();
	return true;
}

bool LL::Expr3() {
	auto tempGraph = graphIt;
	nextGraphState(1);
	generateString("E2");

	if (!Expr2()) return false;

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E3'");

	if (!Expr3List()) return false;

	rollbackIter();
	return true;
}

bool LL::Expr3List() {
	if (it->first == "opmul") {
		nextToken();
		nextGraphState(1);
		generateString("opmul E2");

		auto tempGraph = graphIt;

		if (!Expr2()) return false;

		nextGraphState(0);
		generateString("E3'");

		if (!Expr3List()) return false;

	}

	rollbackIter();
	return true;
}

bool LL::Expr2() {
	auto tempGraph = graphIt;

	if (it->first == "opnot") {
		nextToken();
		nextGraphState(0);
		generateString("opnot E1");

		if (!Expr1()) return false;

		rollbackIter();
	} else {
		nextGraphState(0);
		generateString("E1");

		if (!Expr1()) return false;

		rollbackIter();
	}

	rollbackIter();
	return true;
}

bool LL::Expr1() {
	auto tempGraph = graphIt;

	if (it->first == "opinc") {
		nextToken();
		nextGraphState(1);
		generateString("opinc");

		rollbackIter();

		nextGraphState(0);
		if (it->first != "id") return false;

		generateString(" " + it->second);

		rollbackIter();

		nextToken();
		return true;
	} else if (it->first == "lpar") {

		nextGraphState(1);
		generateString("lpar E");

		nextToken();

		if (!Expr()) return false;

		nextGraphState(0);
		if (it->first != "rpar") return false;

		generateString("rpar");

		rollbackIter();

		nextToken();
		return true;
	} else if (it->first == "num") {
		nextGraphState(0);
		generateString(" " + it->second);
		nextToken();
		rollbackIter();
		return true;
	} else if (it->first == "id") {
		nextGraphState(0);
		generateString(" " + it->second + " E1'");
		nextToken();

		if (!Expr1List()) return false;

		rollbackIter();
		return true;
	}

	return false;
}

bool LL::Expr1List() {
	if (it->first == "lpar") {
		nextToken();

		auto tempGraph = graphIt;
		nextGraphState(1);
		generateString("lpar ArgList");

		if (!ArgList()) return false;

		tempGraph = graphIt;

		if (it->first != "rpar") return false;

		nextGraphState(0);
		generateString("rpar");

		nextToken();
		rollbackIter();
	}

	if (it->first == "opinc") {
		nextToken();

		nextGraphState(0);
		generateString("opinc");
		rollbackIter();
	}

	return true;
}

bool LL::ArgList() {
	if (it->first == "id") {
		nextGraphState(0);
		generateString(" " + it->second + " ArgList'");

		nextToken();

		auto tempGraph = graphIt;

		if (!ArgListList()) return false;

		rollbackIter();
		return true;
	}

	rollbackIter();
	return true;
}

bool LL::ArgListList() {
	if (it->first == "comma") {
		auto tempGraph = graphIt;

		nextToken();

		nextGraphState(0);
		if (it->first != "id") return false;

		generateString("comma " + it->second + " ArgList'");

		nextToken();
		tempGraph = graphIt;

		if (!ArgListList()) return false;
	}

	rollbackIter();
	return true;
}