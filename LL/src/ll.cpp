#include "ll.h"
#include <iostream>
#include <fstream>

LL::LL(std::istream &stream, std::string inputPath) : lexer {stream} {}

LL::~LL() = default;

void LL::validate() {
	nextToken();
	graphIt = states.begin();

	std::ofstream myStream;

	myStream.open(R"(E:\CLionProjects\TvSU_Translator\output.txt)");
//	myStream.clear();

	// TODO: Change Expr() back to StmtList()
	// TODO: Change generateString argument to StmtList()
	generateString("E");
	if (Expr() && it->first == "eof") {
		while (!finalOutput.empty()) {
			myStream << finalOutput.front() << std::endl;
			finalOutput.pop();
		}

		std::cout << "\nAccepted!" << std::endl;
	} else {
		while (!finalOutput.empty()) {
			myStream << finalOutput.front() << std::endl;
			finalOutput.pop();
		}

		std::cout << "\nIncorrect Expression!" << std::endl;
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

		// TODO: delete this line
		nextGraphState(0);

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

void LL::generateString(const std::string &abiba) {
	std::string aboba;
	for (auto i = states.begin(); i != states.end(); i++) {
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
			aboba += "  ";
		}
	}

	finalOutput.emplace(aboba + abiba);
}

void LL::rollbackIter() {
	states.pop_back();
	graphIt = states.end() - 1;
}

bool LL::Expr() {
	auto tempGraph = graphIt;

	nextGraphState(0);
	generateString("E7");
	if (!Expr7()) {
		eraseTrash(tempGraph);
		return false;
	}

	rollbackIter();
	return true;
}

bool LL::Expr7() {
	auto tempGraph = graphIt;

	nextGraphState(1);
	generateString("E6");
	if (!Expr6()) {
		eraseTrash(tempGraph);
		return false;
	}

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E7'");
	if (!Expr7List()) {
		eraseTrash(tempGraph);
		return false;
	}

	rollbackIter();
	return true;
}

bool LL::Expr7List() {
	if (it->first == "opor") {
		nextToken();
		nextGraphState(1);
		generateString("opor E6");

		auto tempGraph = graphIt;

		if (!Expr6()) {
			eraseTrash(tempGraph);
			return false;
		}

		tempGraph = graphIt;

		nextGraphState(0);
		generateString("E7'");
		if (!Expr7List()) {
			eraseTrash(tempGraph);
			return false;
		}
	}

	rollbackIter();
	return true;
}

bool LL::Expr6() {
	auto tempGraph = graphIt;

	nextGraphState(1);
	generateString("E5");
	if (!Expr5()) {
		eraseTrash(tempGraph);
		return false;
	}

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E6'");
	if (!Expr6List()) {
		eraseTrash(tempGraph);
		return false;
	}

	rollbackIter();
	return true;
}

bool LL::Expr6List() {
	if (it->first == "opand") {
		nextToken();
		nextGraphState(1);
		generateString("opand E5");

		auto tempGraph = graphIt;

		if (!Expr5()) {
			eraseTrash(tempGraph);
			return false;
		}

		tempGraph = graphIt;

		nextGraphState(0);
		generateString("E6'");
		if (!Expr6List()) {
			eraseTrash(tempGraph);
			return false;
		}
	}

	rollbackIter();
	return true;
}

bool LL::Expr5() {
	auto tempGraph = graphIt;

	nextGraphState(1);
	generateString("E4");
	if (!Expr4()) {
		eraseTrash(tempGraph);
		return false;
	}

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E5'");
	if (!Expr5List()) {
		eraseTrash(tempGraph);
		return false;
	}

	rollbackIter();
	return true;
}

bool LL::Expr5List() {
	if (it->first == "opeq") {
		nextToken();
		nextGraphState(0);
		generateString("opeq E4");

		auto tempGraph = graphIt;

		if (!Expr4()) {
			eraseTrash(tempGraph);
			return false;
		}

	} else if (it->first == "opne") {
		nextToken();
		nextGraphState(0);
		generateString("opne E4");

		auto tempGraph = graphIt;

		if (!Expr4()) {
			eraseTrash(tempGraph);
			return false;
		}

	} else if (it->first == "ople") {
		nextToken();
		nextGraphState(0);
		generateString("ople E4");

		auto tempGraph = graphIt;

		if (!Expr4()) {
			eraseTrash(tempGraph);
			return false;
		}

	} else if (it->first == "opgt") {
		nextToken();
		nextGraphState(0);
		generateString("opgt E4");

		auto tempGraph = graphIt;

		if (!Expr4()) {
			eraseTrash(tempGraph);
			return false;
		}

	} else if (it->first == "opge") {
		nextToken();
		nextGraphState(0);
		generateString("opge E4");

		auto tempGraph = graphIt;

		if (!Expr4()) {
			eraseTrash(tempGraph);
			return false;
		}

	} else if (it->first == "oplt") {
		nextToken();
		nextGraphState(0);
		generateString("oplt E4");

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
	auto tempGraph = graphIt;
	nextGraphState(1);
	generateString("E3");

	if (!Expr3()) {
		eraseTrash(tempGraph);
		return false;
	}

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E4'");
	if (!Expr4List()) {
		eraseTrash(tempGraph);
		return false;
	}

	rollbackIter();
	return true;
}

bool LL::Expr4List() {
	if (it->first == "opplus") {
		nextToken();
		nextGraphState(1);
		generateString("opplus E3");

		auto tempGraph = graphIt;

		if (!Expr3()) {
			eraseTrash(tempGraph);
			return false;
		}

		tempGraph = graphIt;

		nextGraphState(0);
		generateString("E4'");

		if (!Expr4List()) {
			eraseTrash(tempGraph);
			return false;
		}

	} else if (it->first == "opminus") {
		nextToken();
		nextGraphState(1);
		generateString("opminus E3");

		auto tempGraph = graphIt;

		if (!Expr3()) {
			eraseTrash(tempGraph);
			return false;
		}

		tempGraph = graphIt;

		nextGraphState(0);
		generateString("E4'");

		if (!Expr4List()) {
			eraseTrash(tempGraph);
			return false;
		}

	}

	rollbackIter();
	return true;
}

bool LL::Expr3() {
	auto tempGraph = graphIt;
	nextGraphState(1);
	generateString("E2");

	if (!Expr2()) {
		eraseTrash(tempGraph);
		return false;
	}

	tempGraph = graphIt;

	nextGraphState(0);
	generateString("E3'");

	if (!Expr3List()) {
		eraseTrash(tempGraph);
		return false;
	}

	rollbackIter();
	return true;
}

bool LL::Expr3List() {
	if (it->first == "opmul") {
		nextToken();
		nextGraphState(1);
		generateString("opmul E2");

		auto tempGraph = graphIt;

		if (!Expr2()) {
			eraseTrash(tempGraph);
			return false;
		}

		nextGraphState(0);
		generateString("E3'");

		if (!Expr3List()) {
			eraseTrash(tempGraph);
			return false;
		}

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

		if (!Expr1()) {
			eraseTrash(tempGraph);
			return false;
		}

		rollbackIter();
	} else {
		nextGraphState(0);
		generateString("E1");

		if (!Expr1()) {
			eraseTrash(tempGraph);
			return false;
		}

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
		if (it->first != "id") {
			eraseTrash(tempGraph);
			return false;
		}

		generateString(it->second);

		rollbackIter();

		nextToken();
		return true;
	} else if (it->first == "lpar") {
		nextToken();
		nextGraphState(1);
		generateString("lpar E");

		if (!Expr()) {
			eraseTrash(tempGraph);
			return false;
		}

		nextGraphState(0);
		if (it->first != "rpar") {
			eraseTrash(tempGraph);
			return false;
		}

		generateString("rpar");

		rollbackIter();

		nextToken();
		return true;
	} else if (it->first == "num") {
		nextGraphState(0);
		generateString(it->second);
		nextToken();
		rollbackIter();
		return true;
	} else if (it->first == "id") {
		nextGraphState(0);
		generateString(it->second + " E1'");
		nextToken();

		if (!Expr1List()) {
			eraseTrash(tempGraph);
			return false;
		}

		rollbackIter();
		return true;
	}

	eraseTrash(tempGraph);
	return false;
}

bool LL::Expr1List() {
	auto tempGraph = graphIt;

	if (it->first == "lpar") {
		nextToken();
		nextGraphState(1);
		generateString("lpar ArgList");

		if (!ArgList()) {
			eraseTrash(tempGraph);
			return false;
		}

		tempGraph = graphIt;

		nextGraphState(0);
		if (it->first != "rpar") {
			eraseTrash(tempGraph);
			return false;
		}

		generateString("rpar");

		nextToken();
		rollbackIter();
	} else if (it->first == "opinc") {
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
		generateString(it->second + " ArgListList");

		nextToken();

		auto tempGraph = graphIt;

		if (!ArgListList()) {
			eraseTrash(tempGraph);
			return false;
		}
	}

	rollbackIter();
	return true;
}

bool LL::ArgListList() {
	if (it->first == "comma") {
		auto tempGraph = graphIt;

		nextGraphState(1);
		generateString("comma");
		nextToken();

		rollbackIter();

		nextGraphState(0);
		if (it->first != "id") {
			eraseTrash(tempGraph);
			return false;
		}

		generateString(it->second + " ArgListList");

		nextToken();
		tempGraph = graphIt;

		if (!ArgListList()) {
			eraseTrash(tempGraph);
			return false;
		}
	}

	rollbackIter();
	return true;
}