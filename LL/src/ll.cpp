#include "ll.h"
#include <iostream>

LL::LL(std::istream &stream, const std::string &inputPath) : lexer {stream} {
	_input = inputPath;
}

void LL::validate() {
	nextToken();
	graphIt = states.begin();

	myStream.open(_input);

	generateString("StmtList");
	if (StmtList() && it->first == "eof") {
		while (!outputVector.empty()) {
			myStream << outputVector.front() << std::endl;
			outputVector.erase(outputVector.begin());
		}

		std::cout << "Accepted!" << std::endl;
	} else {
		while (!outputVector.empty()) {
			myStream << outputVector.front() << std::endl;
			outputVector.erase(outputVector.begin());
		}

		std::cout << "Incorrect Expression!" << std::endl;
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

	if (it->first == "eof") {
		eraseTrash(tempGraph);
		return true;
	}

	auto tempIt = it;
	tempGraph = graphIt;

	nextGraphState(1);
	generateString("Stmt");

	if (Stmt()) {

		tempGraph = graphIt;

		nextGraphState(0);
		generateString("StmtList");

		if (!StmtList()) {
			eraseTrash(tempGraph);
			return false;
		}
	} else {
		rollBackChanges(tempIt);
		rollbackIter();

		nextGraphState(0);
		generateString("StmtList");

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

	nextGraphState(1);
	generateString("DeclareStmt");

	tempCnt = outVecCnt;

	rollbackIter();

	// TODO: Make it done

	if (AssignOrCallOp()) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
	}

	rollbackIter();

	nextGraphState(1);
	generateString("AssignOrCallOp");

	tempCnt = outVecCnt;

	rollbackIter();

	if (WhileOp()) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
	}

	nextGraphState(1);
	generateString("WhileOp");

	tempCnt = outVecCnt;

	rollbackIter();

	// TODO: Make it done

	if (ForOp()) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
	}

	nextGraphState(1);
	generateString("ForOp");

	tempCnt = outVecCnt;

	rollbackIter();

	if (IfOp()) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
		rollbackIter();
	}

	nextGraphState(1);
	generateString("IfOp");

	tempCnt = outVecCnt;

	rollbackIter();

	// TODO: Make it done

	if (SwitchOp()) {
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
	}

	nextGraphState(1);
	generateString("SwitchOp");

	tempCnt = outVecCnt;

	rollbackIter();

	if (InOp()) {
		rollbackIter();
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
	}

	nextGraphState(1);
	generateString("InOp");

	tempCnt = outVecCnt;

	rollbackIter();

	if (OutOp()) {
		rollbackIter();
		return true;
	} else {
		outputVector.erase(outputVector.end() - outVecCnt + tempCnt, outputVector.end());
		outVecCnt = tempCnt;
		rollBackChanges(tempIt);
	}

	nextGraphState(1);
	generateString("OutOp");

	rollbackIter();

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

		int tempCnt = outVecCnt;

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

	outputVector.erase(outputVector.end() - 7 - outVecCnt + tempCnt, outputVector.end());
	return false;
}

// TODO: Make it done

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

// TODO: Make it done

bool LL::Cases() {
	if (!ACase()) return false;
	if (!CasesList()) return false;
	return true;
}

// TODO: Make it done

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

// TODO: Make it done

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

// TODO: Make it done

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

// TODO: Make it done

void LL::ForInit() {
	auto tempIt = it;
	if (!Type()) rollBackChanges(tempIt);
	if (AssignOrCall()) {
		return;
	} else {
		rollBackChanges(tempIt);
	}
}

// TODO: Make it done

void LL::ForExp() {
	auto tempIt = it;

	if (Expr()) {
		return;
	} else {
		rollBackChanges(tempIt);
	}

}

// TODO: Make it done

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

	nextGraphState(0);
	generateString("IfOp");

	if (it->first != "kwif") return false;
	nextToken();

	nextGraphState(1);
	generateString("kwif");
	rollbackIter();

	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("lpar E");

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

// TODO: Make it done

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

// TODO: Make it done

bool LL::AssignOrCall() {
	if (it->first != "id") return false;

	nextGraphState(0);
	generateString(it->second + " AssignOrCall'");

	nextToken();
	if (!AssignOrCallList()) return false;

	rollbackIter();
	return true;
}

// TODO: Make it done

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

	nextGraphState(1);
	generateString("kwwhile");
	rollbackIter();

	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("lpar E");

	if (!Expr()) return false;

	if (it->first != "rpar") return false;
	nextToken();

	nextGraphState(0);
	generateString("rpar Stmt");

	if (!Stmt()) return false;

	rollbackIter();
	rollbackIter();
	return true;
}

bool LL::InOp() {
	auto tempGraph = graphIt;

	if (it->first != "kwin") {
		eraseTrash(tempGraph);
		return false;
	}

	nextGraphState(0);
	generateString("InOp");

	nextGraphState(1);
	generateString("kwin");
	rollbackIter();

	nextToken();

	tempGraph = graphIt;

	if (it->first != "id") {
		eraseTrash(tempGraph);
		return false;
	}

	nextGraphState(1);
	generateString(it->second);
	rollbackIter();

	nextToken();

	tempGraph = graphIt;

	if (it->first != "semicolon") {
		eraseTrash(tempGraph);
		return false;
	}

	nextGraphState(0);
	generateString("semicolon");
	rollbackIter();

	nextToken();

	rollbackIter();
	return true;
}

bool LL::OutOp() {
	auto tempGraph = graphIt;

	if (it->first != "kwout") {
		eraseTrash(tempGraph);
		return false;
	}

	nextGraphState(0);
	generateString("OutOp");

	nextGraphState(1);
	generateString("kwout E");

	nextToken();

	tempGraph = graphIt;

	if (!Expr()) {
		eraseTrash(tempGraph);
		return false;
	}

	tempGraph = graphIt;

	if (it->first != "semicolon") {
		eraseTrash(tempGraph);
		return false;
	}

	nextGraphState(0);
	generateString("semicolon");
	rollbackIter();

	nextToken();
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
	generateString(it->second + " DeclareStmt'");
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

		nextGraphState(1);
		generateString("rpar");
		rollbackIter();
		nextToken();

		if (it->first != "lbrace") return false;

		nextGraphState(1);
		generateString("lbrace StmtList");
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

		nextGraphState(1);
		generateString("opassign");
		rollbackIter();

		if (it->first == "num") {
			nextGraphState(1);
			generateString(it->second + " DeclareVarList");
			nextToken();

			if (!DeclareVarList()) return false;

			rollbackIter();

			if (it->first != "semicolon") return false;

			nextGraphState(0);
			generateString("semicolon");
			nextToken();

			rollbackIter();

			rollbackIter();
			return true;
		} else if (it->first == "char") {
			nextGraphState(1);
			generateString(it->second + " DeclareVarList");
			nextToken();

			if (!DeclareVarList()) return false;

			rollbackIter();

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

		nextGraphState(1);
		generateString("comma");
		rollbackIter();
		nextToken();

		if (it->first != "id") return false;

		nextGraphState(1);
		generateString(it->second + " InitVar");
		nextToken();

		if (!InitVar()) return false;

		rollbackIter();
		nextGraphState(1);
		generateString("DeclareVarList");

		if (!DeclareVarList()) return false;

		rollbackIter();
	}

	return true;
}

bool LL::InitVar() {
	if (it->first == "eof") return false;

	if (it->first == "opassign") {
		nextToken();

		nextGraphState(1);
		generateString("opassign");
		rollbackIter();

		if (it->first == "num" ||
		    it->first == "char")
		{
			nextGraphState(0);
			generateString(it->second);
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
		generateString(it->second + " ParamList'");
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
		generateString(it->second + " ParamList'");
		nextToken();

		if (!ParamListList()) return false;
	}

	rollbackIter();
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

// TODO: Add ArgList

bool LL::Expr1List() {
//	if (it->first == "lpar") {
//		nextToken();
//		nextGraphState(1);
//		generateString("lpar ArgList");
//
//		auto tempGraph = graphIt;
//
//		if (!ArgList()) {
//			eraseTrash(tempGraph);
//			return false;
//		}
//
//		tempGraph = graphIt;
//
//		nextGraphState(0);
//		if (it->first != "rpar") {
//			eraseTrash(tempGraph);
//			return false;
//		}
//
//		generateString("rpar");
//
//		nextToken();
//		rollbackIter();
//	} else if (it->first == "opinc") {
//		nextToken();
//
//		nextGraphState(0);
//		generateString("opinc");
//		rollbackIter();
//	}

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
		generateString(it->second + " ArgList'");

		nextToken();

		auto tempGraph = graphIt;

		if (!ArgListList()) {
			eraseTrash(tempGraph);
			return false;
		}

		rollbackIter();
		return true;
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

		generateString(it->second + " ArgList'");

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