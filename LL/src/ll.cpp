#include "ll.h"
#include <iostream>

LL::LL(std::istream &stream) : lexer {stream} {}

LL::~LL() = default;

void LL::validate() {
	nextToken();
	graphIt = states.begin();

	if (StmtList() && it->first == "eof") {
		while (!finalOutput.empty()) {
			std::cout << finalOutput.front() << std::endl;
			finalOutput.pop();
		}

		std::cout << "\nAccepted!" << std::endl;
	} else {
		std::cout << "\nIncorrect Expression!" << std::endl;
	}
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
		if (!StmtList()) return false;
		return true;
	}

	if (it->first == "kwdefault") {
		nextToken();
		if (it->first != "colon") return false;
		nextToken();
		if (!StmtList()) return false;
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
	if (!Type()) rollBackChanges(tempIt);
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

void LL::eraseTrash(const std::vector<int>::iterator &da) {
	states.erase(da, graphIt);
	graphIt = da;
}

std::string LL::generateString() {
	std::string aboba;
	for (auto i = states.begin(); i != states.end(); i++) {
		if (i == states.end() - 1) {
			if (*i == 1) {
				aboba += "|- ";
			} else {
				aboba += "L ";
			}

			break;
		}

		if (*i == 1) {
			aboba += "|  ";
		} else {
			aboba += "  ";
		}
	}

	return aboba;
}

void LL::rollbackIter() {
	graphIt--;
	states.pop_back();
}

bool LL::Expr() {
	nextGraphState(0);
	std::string newStackString = generateString() + "E";
	finalOutput.emplace(newStackString);

	auto tempGraph = graphIt;

	if (!Expr7()) {
		eraseTrash(tempGraph);
		return false;
	}

	rollbackIter();
	return true;
}

bool LL::Expr7() {
	nextGraphState(0);
	std::string newStackString = generateString() + "E7";
	finalOutput.emplace(newStackString);

	auto tempGraph = graphIt;

	if (!Expr6()) {
		eraseTrash(tempGraph);
		return false;
	}

	tempGraph = graphIt;

	if (!Expr7List()) {
		eraseTrash(tempGraph);
		return false;
	}

	rollbackIter();
	return true;
}

bool LL::Expr7List() {
	nextGraphState(0);
	std::string newStackString = generateString() + "E7'";
	finalOutput.emplace(newStackString);

	if (it->first == "opor") {
		nextToken();

		std::string huy = generateString() + "|| E6";
		auto tempGraph = graphIt;

		if (!Expr6()) {
			eraseTrash(tempGraph);
			return false;
		}

		tempGraph = graphIt;

		if (!Expr7List()) {
			eraseTrash(tempGraph);
			return false;
		}
	}

	rollbackIter();
	return true;
}

bool LL::Expr6() {
	nextGraphState(1);
	std::string newStackString = generateString() + "E6";
	finalOutput.emplace(newStackString);

	auto tempGraph = graphIt;

	if (!Expr5()) {
		eraseTrash(tempGraph);
		return false;
	}

	tempGraph = graphIt;

	if (!Expr6List()) {
		eraseTrash(tempGraph);
		return false;
	}

	rollbackIter();
	return true;
}

bool LL::Expr6List() {
	nextGraphState(0);
	std::string newStackString = generateString() + "E6'";
	finalOutput.emplace(newStackString);

	if (it->first == "opand") {
		nextToken();

		std::string huy = generateString() + "&& E5";
		auto tempGraph = graphIt;

		if (!Expr5()) {
			eraseTrash(tempGraph);
			return false;
		}

		tempGraph = graphIt;

		if (!Expr6List()) {
			eraseTrash(tempGraph);
			return false;
		}
	}

	rollbackIter();
	return true;
}

bool LL::Expr5() {
	nextGraphState(1);
	std::string newStackString = generateString() + "E5";
	finalOutput.emplace(newStackString);

	nextGraphState(1);
	auto tempGraph = graphIt;

	if (!Expr4()) {
		eraseTrash(tempGraph);
		return false;
	}

	rollbackIter();
	nextGraphState(0);
	tempGraph = graphIt;

	if (!Expr5List()) {
		eraseTrash(tempGraph);
		return false;
	}

	rollbackIter();
	return true;
}

bool LL::Expr5List() {
	std::string newStackString = generateString() + "E5'";
	finalOutput.emplace(newStackString);

	if (it->first == "opeq") {
		nextToken();

		std::string huy = generateString() + "== E4";
		nextGraphState(0);
		auto tempGraph = graphIt;

		if (!Expr4()) {
			eraseTrash(tempGraph);
			return false;
		}
	} else if (it->first == "opne") {
		nextToken();

		std::string huy = generateString() + "!= E4";
		nextGraphState(0);
		auto tempGraph = graphIt;

		if (!Expr4()) {
			eraseTrash(tempGraph);
			return false;
		}
	} else if (it->first == "ople") {
		nextToken();

		std::string huy = generateString() + "<= E4";
		nextGraphState(0);
		auto tempGraph = graphIt;

		if (!Expr4()) {
			eraseTrash(tempGraph);
			return false;
		}
	} else if (it->first == "opgt") {
		nextToken();

		std::string huy = generateString() + "> E4";
		nextGraphState(0);
		auto tempGraph = graphIt;

		if (!Expr4()) {
			eraseTrash(tempGraph);
			return false;
		}
	} else if (it->first == "opge") {
		nextToken();

		std::string huy = generateString() + ">= E4";
		nextGraphState(0);
		auto tempGraph = graphIt;

		if (!Expr4()) {
			eraseTrash(tempGraph);
			return false;
		}
	} else if (it->first == "oplt") {
		nextToken();

		std::string huy = generateString() + "< E4";
		nextGraphState(0);
		auto tempGraph = graphIt;

		if (!Expr4()) {
			eraseTrash(tempGraph);
			return false;
		}
	}

	rollbackIter();
	return true;
}

bool LL::Expr4() {
	if (!Expr3()) return false;
	if (!Expr4List()) return false;
	return true;
}

bool LL::Expr4List() {
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
		if (!Expr1List()) return false;
		return true;
	}

	return false;
}

bool LL::Expr1List() {
	if (it->first == "lpar") {
		nextToken();
		if (!ArgList()) return false;
		if (it->first != "rpar") return false;
		nextToken();
	} else if (it->first == "opinc") {
		nextToken();
	}

	return true;
}

bool LL::ArgList() {
	if (it->first == "id") {
		nextToken();
		if (!ArgListList()) return false;
	}

	return true;
}

bool LL::ArgListList() {
	if (it->first == "comma") {
		nextToken();
		if (it->first != "id") return false;
		nextToken();
		if (!ArgListList()) return false;
	}

	return true;
}