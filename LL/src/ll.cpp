#include "ll.h"

LL::LL(std::istream &stream) : lexer {stream} {}

LL::~LL() = default;

std::string LL::validate() {
	nextToken();

	if (StmtList() && it->first == "eof") {
		return "Accepted!";
	}

	return "Incorrect Expression!";
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
	if (it->first == "eof") return true;

	auto tempIt = it;
	if (Stmt()) {
		if (!StmtList()) return false;
	} else {
		rollBackChanges(tempIt);
	}

	return true;
}

bool LL::Stmt() {
	if (it->first == "eof") return false;

	auto tempIt = it;
	if (DeclareStmt()) {
		return true;
	} else {
		rollBackChanges(tempIt);
	}

	if (AssignOrCallOp()) {
		return true;
	} else {
		rollBackChanges(tempIt);
	}

	if (WhileOp()) {
		return true;
	} else {
		rollBackChanges(tempIt);
	}

	if (ForOp()) {
		return true;
	} else {
		rollBackChanges(tempIt);
	}

	if (IfOp()) {
		return true;
	} else {
		rollBackChanges(tempIt);
	}

	if (SwitchOp()) {
		return true;
	} else {
		rollBackChanges(tempIt);
	}

	if (InOp()) {
		return true;
	} else {
		rollBackChanges(tempIt);
	}

	if (OutOp()) {
		return true;
	} else {
		rollBackChanges(tempIt);
	}

	if (it->first == "semicolon") {
		nextToken();
		return true;
	}

	else if (it->first == "lbrace") {
		nextToken();
		if (!StmtList()) return false;
		if (it->first != "rbrace") return false;
		nextToken();
		return true;
	}

	else if (it->first == "kwreturn") {
		nextToken();
		if (!Expr()) return false;
		if (it->first != "semicolon") return false;
		nextToken();
		return true;
	}

	return false;
}

bool LL::SwitchOp() {
	if (it->first != "kwswitch") return false;
	nextToken();
	if (it->first != "lpar") return false;
	nextToken();
	if (!Expr()) return false;
	if (it->first != "rpar") return false;
	nextToken();
	if (it->first != "lbrace") return false;
	nextToken();
	if (!Cases()) return false;
	if (it->first != "rbrace") return false;
	nextToken();
	return true;
}

bool LL::Cases() {
	if (!ACase()) return false;
	if (!CasesList()) return false;
	return true;
}

bool LL::CasesList() {
	auto tempIt = it;
	if (ACase()) {
		if (!CasesList()) return false;
		return true;
	} else {
		rollBackChanges(tempIt);
	}

	return true;
}

bool LL::ACase() {
	if (it->first == "kwcase") {
		nextToken();
		if (it->first != "num") return false;
		nextToken();
		if (it->first != "colon") return false;
		nextToken();
		if (!Stmt()) return false;
		return true;
	}

	if (it->first == "kwdefault") {
		nextToken();
		if (it->first != "colon") return false;
		nextToken();
		if (!Stmt()) return false;
		return true;
	}

	return false;
}

bool LL::ForOp() {
	if (it->first != "kwfor") return false;
	nextToken();
	if (it->first != "lpar") return false;
	nextToken();
	ForInit();
	if (it->first != "semicolon") return false;
	nextToken();
	ForExp();
	if (it->first != "semicolon") return false;
	nextToken();
	if (!ForLoop()) return false;
	if (it->first != "rpar") return false;
	nextToken();
	if (!Stmt()) return false;
	return true;
}

void LL::ForInit() {
	auto tempIt = it;
	if (AssignOrCall()) {
		return;
	} else {
		rollBackChanges(tempIt);
	}
}

void LL::ForExp() {
	auto tempIt = it;

	if (Expr()) {
		return;
	} else {
		rollBackChanges(tempIt);
	}

}

bool LL::ForLoop() {
	if (it->first == "opinc") {
		nextToken();
		if (it->first != "id") return false;
		nextToken();
		return true;
	}

	auto tempIt = it;

	if (AssignOrCall()) {
		return true;
	} else {
		rollBackChanges(tempIt);
	}

	return true;
}

bool LL::IfOp() {
	if (it->first != "kwif") return false;
	nextToken();
	if (it->first != "lpar") return false;
	nextToken();
	if (!Expr()) return false;
	if (it->first != "rpar") return false;
	nextToken();
	if (!Stmt()) return false;
	if (!ElsePart()) return false;
	return true;
}

bool LL::ElsePart() {
	if (it->first == "kwelse") {
		nextToken();
		if (!Stmt()) return false;
	}

	return true;
}

bool LL::AssignOrCallOp() {
	if (!AssignOrCall()) return false;
	if (it->first != "semicolon") return false;
	nextToken();
	return true;
}

bool LL::AssignOrCall() {
	if (it->first != "id") return false;
	nextToken();
	if (!AssignOrCallList()) return false;
	return true;
}

bool LL::AssignOrCallList() {
	if (it->first == "opassign") {
		nextToken();
		if (!Expr()) return false;
		return true;
	}

	if (it->first == "lpar") {
		nextToken();
		if (!ParamList()) return false;
		if (it->first != "rpar") return false;
		nextToken();
		return true;
	}

	return false;
}

bool LL::WhileOp() {
	if (it->first != "kwwhile") return false;
	nextToken();
	if (it->first != "lpar") return false;
	nextToken();
	if (!Expr()) return false;
	if (it->first != "rpar") return false;
	nextToken();
	if (!Stmt()) return false;
	return true;
}

bool LL::InOp() {
	if (it->first != "kwin") return false;
	nextToken();
	if (it->first != "id") return false;
	nextToken();
	if (it->first != "semicolon") return false;
	nextToken();
	return true;
}

bool LL::OutOp() {
	if (it->first != "kwout") return false;
	nextToken();
	if (!Expr()) return false;
	if (it->first != "semicolon") return false;
	nextToken();
	return true;
}

bool LL::DeclareStmt() {
	if (it->first == "eof") return false;

	if (!Type()) return false;
	if (it->first != "id") return false;
	nextToken();
	if (!DeclareStmtList()) return false;

	return true;
}

bool LL::Type() {
	if (it->first == "eof") return false;

	if (it->first == "kwint" ||
	    it->first == "kwchar")
	{
		nextToken();
		return true;
	}

	return false;
}

bool LL::DeclareStmtList() {
	if (it->first == "eof") return false;

	if (it->first == "lpar") {
		nextToken();
		if (!ParamList()) return false;
		if (it->first != "rpar") return false;
		nextToken();
		if (it->first != "lbrace") return false;
		nextToken();
		if (!StmtList()) return false;
		if (it->first != "rbrace") return false;
		nextToken();
		return true;
	} else if (it->first == "opassign") {
		nextToken();
		if (it->first == "num") {
			nextToken();
			if (!DeclareVarList()) return false;
			if (it->first != "semicolon") return false;
			nextToken();
			return true;
		} else if (it->first == "char") {
			nextToken();
			if (!DeclareVarList()) return false;
			if (it->first != "semicolon") return false;
			nextToken();
			return true;
		} else {
			return false;
		}
	} else {
		if (!DeclareVarList()) return false;
		if (it->first != "semicolon") return false;
		nextToken();
		return true;
	}
}

bool LL::DeclareVarList() {
	if (it->first == "eof") return false;

	if (it->first == "comma") {
		nextToken();
		if (it->first != "id") return false;
		nextToken();
		if (!InitVar()) return false;
		if (!DeclareVarList()) return false;
	}

	return true;
}

bool LL::InitVar() {
	if (it->first == "eof") return false;

	if (it->first == "opassign") {
		nextToken();
		if (it->first == "num" ||
		    it->first == "char")
		{
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

	if (Type()) {
		if (it->first != "id") return false;
		nextToken();
		if (!ParamListList()) return false;
	} else {
		rollBackChanges(tempIt);
	}

	return true;
}

bool LL::ParamListList() {
	if (it->first == "eof") return false;

	if (it->first == "comma") {
		nextToken();
		if (!Type()) return false;
		if (it->first != "id") return false;
		nextToken();
		if (!ParamListList()) return false;
	}

	return true;
}

bool LL::Expr() {
	if (!Expr7()) return false;
	return true;
}

bool LL::Expr7() {
	if (!Expr6()) return false;
	if (!Expr7List()) return false;
	return true;
}

bool LL::Expr7List() {
	if (it->first == "eof") return true;

	if (it->first == "opor") {
		nextToken();
		if (!Expr6()) return false;
		if (!Expr7List()) return false;
	}

	return true;
}

bool LL::Expr6() {
	if (!Expr5()) return false;
	if (!Expr6List()) return false;
	return true;
}

bool LL::Expr6List() {
	if (it->first == "eof") return true;

	if (it->first == "opand") {
		nextToken();
		if (!Expr5()) return false;
		if (!Expr6List()) return false;
	}

	return true;
}

bool LL::Expr5() {
	if (!Expr4()) return false;
	if (!Expr5List()) return false;
	return true;
}

bool LL::Expr5List() {
	if (it->first == "eof") return true;

	if (it->first == "opeq") {
		nextToken();
		if (!Expr4()) return false;
	} else if (it->first == "opne") {
		nextToken();
		if (!Expr4()) return false;
	} else if (it->first == "ople") {
		nextToken();
		if (!Expr4()) return false;
	} else if (it->first == "opgt") {
		nextToken();
		if (!Expr4()) return false;
	} else if (it->first == "opge") {
		nextToken();
		if (!Expr4()) return false;
	} else if (it->first == "oplt") {
		nextToken();
		if (!Expr4()) return false;
	}

	return true;
}

bool LL::Expr4() {
	if (!Expr3()) return false;
	if (!Expr4List()) return false;
	return true;
}

bool LL::Expr4List() {
	if (it->first == "eof") return true;

	if (it->first == "opplus") {
		nextToken();
		if (!Expr3()) return false;
		if (!Expr4List()) return false;
	} else if (it->first == "opminus") {
		nextToken();
		if (!Expr3()) return false;
		if (!Expr4List()) return false;
	}

	return true;
}

bool LL::Expr3() {
	if (!Expr2()) return false;
	if (!Expr3List()) return false;
	return true;
}

bool LL::Expr3List() {
	if (it->first == "eof") return true;

	if (it->first == "opmul") {
		nextToken();
		if (!Expr2()) return false;
		if (!Expr3List()) return false;
	}

	return true;
}

bool LL::Expr2() {
	if (it->first == "opnot") {
		nextToken();
	}

	if (!Expr1()) return false;
	return true;
}

bool LL::Expr1() {
	if (it->first == "eof") return false;

	if (it->first == "opinc") {
		nextToken();
		if (it->first != "id") return false;
		nextToken();
		return true;
	} else if (it->first == "lpar") {
		nextToken();
		if (!Expr()) return false;
		if (it->first != "rpar") return false;
		nextToken();
		return true;
	} else if (it->first == "num") {
		nextToken();
		return true;
	} else if (it->first == "id") {
		nextToken();
		if (it->first == "opinc") nextToken();
		return true;
	}

	return false;
}