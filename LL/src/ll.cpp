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

//  TODO: Implement underlying function
//	if (!AssignOrCallOp()) {
//		rollBackChanges(tempIt);
//	}

//  TODO: Implement underlying function
//	if (!WhileOp()) {
//		rollBackChanges(tempIt);
//	}

//  TODO: Implement underlying function
//	if (!ForOp()) {
//		rollBackChanges(tempIt);
//	}

//  TODO: Implement underlying function
//	if (!IfOp()) {
//		rollBackChanges(tempIt);
//	}

//  TODO: Implement underlying function
//	if (!DeclareStmt()) {
//		rollBackChanges(tempIt);
//	}

//	TODO: Implement underlying function
//	if (!DeclareStmt()) {
//		rollBackChanges(tempIt);
//	}

//	TODO: Implement underlying function
//	if (!DeclareStmt()) {
//		rollBackChanges(tempIt);
//	}

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