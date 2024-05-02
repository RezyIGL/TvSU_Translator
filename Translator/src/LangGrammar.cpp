#include "Translator.h"

// MiniC grammar check
bool LL::StmtList() {
	if (it->first == "eof") return true;

	auto _temp = it;

	nextGraphState(1);
	generateString("Stmt");

	bool result = Stmt();
	if (!result) {
		outputVector.pop_back();
		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	nextGraphState(0);
	generateString("StmtList");

	bool tailResult = StmtList();
	if (!tailResult) return false;

	rollbackGraphNode();
	return true;
}

bool LL::Stmt() {
	if (it->first == "eof") return false;

	if (it->first == "kwint" || it->first == "kwchar") {
		nextGraphState(0);
		generateString("DeclareStmt");

		if (!DeclareStmt()) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "semicolon") {
		nextToken();

		nextGraphState(0);
		generateString("semicolon");

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	if (contextStack.top() == "-1") return false;

	if (it->first == "id") {
		nextGraphState(0);
		generateString("AssignOrCallOp");

		if (!AssignOrCallOp()) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwwhile") {
		nextToken();

		nextGraphState(0);
		generateString("kwwhile WhileOp");

		if (!WhileOp()) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwfor") {
		nextToken();

		nextGraphState(0);
		generateString("kwfor ForOp");

		if (!ForOp()) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwif") {
		nextToken();

		nextGraphState(0);
		generateString("kwif IfOp");

		if (!IfOp()) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwswitch") {
		nextToken();

		nextGraphState(0);
		generateString("kwswitch SwitchOp");

		if (!SwitchOp()) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwin") {
		nextToken();

		nextGraphState(0);
		generateString("kwin InOp");

		if (!InOp()) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwout") {
		nextToken();

		nextGraphState(0);
		generateString("kwout OutOp");

		if (!OutOp()) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "lbrace") {
		nextToken();

		nextGraphState(1);
		generateString("lbrace StmtList");

		bool result = StmtList();
		if (!result) return false;

		if (it->first != "rbrace") return false;
		nextToken();

		nextGraphState(0);
		generateString("rbrace");

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwreturn") {
		nextToken();

		nextGraphState(1);
		generateString("kwreturn E");

		auto EResult = Expr();
		if (!EResult.first) return false;

		generateAtom(contextStack.top(), "RET", "", "", EResult.second);


		if (it->first != "semicolon") return false;
		nextToken();

		nextGraphState(0);
		generateString("semicolon");

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

    if (it->first == "opinc") {
        nextToken();

        if (it->first != "id") return false;

        nextGraphState(0);
        generateString("opinc " + it->second);

        auto q = checkVar(it->second);
        generateAtom(contextStack.top(), "ADD", q, "1", q);

        nextToken();

        rollbackGraphNode();
        rollbackGraphNode();
        return true;
    }

    if (it->first == "opdec") {
        nextToken();

        if (it->first != "id") return false;

        nextGraphState(0);
        generateString("opdec " + it->second);

        auto q = checkVar(it->second);
        generateAtom(contextStack.top(), "SUB", q, "1", q);

        nextToken();

        rollbackGraphNode();
        rollbackGraphNode();
        return true;
    }

	return false;
}

bool LL::DeclareStmt() {

	nextGraphState(1);
	generateString("Type");

	auto TypeResult = Type();
	if (!TypeResult.first) return false;

	if (it->first != "id") return false;
	auto temp = it->second;
	nextToken();

	nextGraphState(0);
	generateString(temp + " DeclareStmt'");

	if (!DeclareStmtList(TypeResult.second, temp)) return false;

	rollbackGraphNode();
	return true;
}

bool LL::DeclareStmtList(const std::string &type, const std::string &name) {
	if (it->first == "eof") return false;

	if (it->first == "lpar") {
		if (contextStack.top() != "-1") return false;
		nextToken();

		nextGraphState(1);
		generateString("lpar ParamList");

		std::string TC = addFunc(name, type);
		contextStack.push(TC);

		FT ParamListRes = ParamList();
		if (!ParamListRes.first) return false;

		for (auto &i : AtomicMap["-1"]) {
			if (std::to_string(i.cnt) == contextStack.top()) {
				i.length = ParamListRes.second;
			}
		}

		if (it->first != "rpar") return false;
		nextToken();

		if (it->first != "lbrace") return false;
		nextToken();

		nextGraphState(1);
		generateString("rpar lbrace StmtList");

        if (!StmtList()) return false;

		if (it->first != "rbrace") return false;
		nextToken();

		generateAtom(contextStack.top(), "RET", "", "", "0");
        contextStack.pop();

		nextGraphState(0);
		generateString("rbrace");

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	} else if (it->first == "opassign") {
		nextToken();
		auto _temp = it->second;

		if (it->first == "num") {
			std::string newVar = "'" + addVar(name, contextStack.top(), type, _temp) + "'";

			if (newVar == "'Error'") return false;

			nextToken();

			nextGraphState(1);
			generateString("opassign " + _temp + " DeclareVarList");

			auto DecVarListRes = DeclareVarList(type);
			if (!DecVarListRes) return false;

			if (it->first != "semicolon") return false;
			nextToken();

			nextGraphState(0);
			generateString("semicolon");

			rollbackGraphNode();
			rollbackGraphNode();
			return true;
		} else if (it->first == "char") {
			std::string newVar = "'" + addVar(name, contextStack.top(), type, _temp) + "'";

			if (newVar == "'Error'") return false;

			nextToken();

			nextGraphState(1);
			generateString("opassign " + _temp + "DeclareVarList");

			auto DecVarListRes = DeclareVarList(type);
			if (!DecVarListRes) return false;

			if (it->first != "semicolon") return false;
			nextToken();

			nextGraphState(0);
			generateString("semicolon");

			rollbackGraphNode();
			rollbackGraphNode();
			return true;
		} else {
			return false;
		}
	} else {
		std::string _temp = addVar(name, contextStack.top(), type);

		if (_temp == "'Error'") return false;

		nextGraphState(1);
		generateString("DeclareVarList");

		if (!DeclareVarList(type)) return false;

		if (it->first != "semicolon") return false;
		nextToken();

		nextGraphState(0);
		generateString("semicolon");

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}
}

bool LL::AssignOrCallOp() {

	nextGraphState(1);
	generateString("AssignOrCall");

	if (!AssignOrCall()) return false;

	if (it->first != "semicolon") return false;
	nextToken();

	nextGraphState(0);
	generateString("semicolon");

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::AssignOrCall() {
	if (it->first != "id") return false;
	auto temp = it->second;
	nextToken();

	nextGraphState(0);
	generateString(temp + " AssignOrCall'");

	if (!AssignOrCallList(temp)) return false;

	rollbackGraphNode();
	return true;
}

bool LL::AssignOrCallList(const std::string &name) {
	if (it->first == "opassign") {
		nextToken();

		nextGraphState(0);
		generateString("opassign E");

		auto ERes = Expr();
		if (!ERes.first) return false;

		auto r = checkVar(name);
		generateAtom(contextStack.top(), "MOV", ERes.second, "", r);

		rollbackGraphNode();
		return true;
	}

    if (it->first == "opinc") {
        nextToken();

        nextGraphState(0);
        generateString("opinc");

        auto s = checkVar(name);
        auto r = alloc(contextStack.top());

        generateAtom(contextStack.top(), "MOV", s, "", r);
        generateAtom(contextStack.top(), "ADD", s, "1", s);

        rollbackGraphNode();
        rollbackGraphNode();
        return true;
    }

    if (it->first == "opdec") {
        nextToken();

        nextGraphState(0);
        generateString("opdec");

        auto s = checkVar(name);
        auto r = alloc(contextStack.top());

        generateAtom(contextStack.top(), "MOV", s, "", r);
        generateAtom(contextStack.top(), "SUB", s, "1", s);

        rollbackGraphNode();
        rollbackGraphNode();
        return true;
    }

	if (it->first == "opmulassign") {
		nextToken();

		nextGraphState(0);
		generateString("opmulassign E");

		auto ERes = Expr();
		if (!ERes.first) return false;

		auto r = checkVar(name);
		auto s = alloc(contextStack.top());

		generateAtom(contextStack.top(), "MUL", r, ERes.second, s);
		generateAtom(contextStack.top(), "MOV", s, "", r);

		rollbackGraphNode();
		return true;
	}

	if (it->first == "opplusassign") {
		nextToken();

		nextGraphState(0);
		generateString("opplusassign E");

		auto ERes = Expr();
		if (!ERes.first) return false;

		auto r = checkVar(name);
		auto s = alloc(contextStack.top());

		generateAtom(contextStack.top(), "ADD", r, ERes.second, s);
		generateAtom(contextStack.top(), "MOV", s, "", r);

		rollbackGraphNode();
		return true;
	}

	if (it->first == "opminusassign") {
		nextToken();

		nextGraphState(0);
		generateString("opminusassign E");

		auto ERes = Expr();
		if (!ERes.first) return false;

		auto r = checkVar(name);
		auto s = alloc(contextStack.top());

		generateAtom(contextStack.top(), "SUB", r, ERes.second, s);
		generateAtom(contextStack.top(), "MOV", s, "", r);

		rollbackGraphNode();
		return true;
	}

	if (it->first == "lpar") {
		nextToken();

		nextGraphState(1);
		generateString("lpar ArgList");

		auto ArgListRes = ArgList();
		if (!ArgListRes.first) return false;

		if (it->first != "rpar") return false;
		nextToken();

		nextGraphState(0);
		generateString("rpar");

		auto q = checkFunc(name, ArgListRes.second);
		auto r = alloc(contextStack.top());
		generateAtom(contextStack.top(), "CALL", q, "", r);

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	return false;
}

bool LL::WhileOp() {

	auto l1 = newLabel();
	auto l2 = newLabel();

	generateAtom(contextStack.top(), "LBL", "", "", "L" + l1);

	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("lpar E");

	auto ERes = Expr();
	if (!ERes.first) return false;

	generateAtom(contextStack.top(), "EQ", ERes.second, "0", "L" + l2);

	if (it->first != "rpar") return false;
	nextToken();

	nextGraphState(0);
	generateString("rpar Stmt");

	if (!Stmt()) return false;

	generateAtom(contextStack.top(), "JMP", "", "", "L" + l1);
	generateAtom(contextStack.top(), "LBL", "", "", "L" + l2);

	rollbackGraphNode();
	return true;
}

bool LL::ForOp() {

	auto l1 = newLabel();
	auto l2 = newLabel();
	auto l3 = newLabel();
	auto l4 = newLabel();

	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("lpar ForInit");

	if (!ForInit()) return false;

	generateAtom(contextStack.top(), "LBL", "", "", "L" + l1);

	nextGraphState(1);
	generateString("semicolon ForExp");

	FT ForExpRes = ForExp();
	if (!ForExpRes.first) return false;

	generateAtom(contextStack.top(), "EQ", ForExpRes.second, "0", "L" + l4);
	generateAtom(contextStack.top(), "JMP", "", "", "L" + l3);
	generateAtom(contextStack.top(), "LBL", "", "", "L" + l2);

	nextGraphState(1);
	generateString("semicolon ForLoop");

	if (!ForLoop()) return false;

	generateAtom(contextStack.top(), "JMP", "", "", "L" + l1);
	generateAtom(contextStack.top(), "LBL", "", "", "L" + l3);

	nextGraphState(0);
	generateString("rpar Stmt");

	if (!Stmt()) return false;

	generateAtom(contextStack.top(), "JMP", "", "", "L" + l2);
	generateAtom(contextStack.top(), "LBL", "", "", "L" + l4);

	rollbackGraphNode();
	return true;
}

bool LL::ForInit() {
	if (it->first == "kwint") {

		nextGraphState(1);
		generateString("kwint");
		rollbackGraphNode();

		nextToken();

		if (it->first != "id") return false;

		std::string _temp = addVar(it->second, contextStack.top(), "kwint");
		if (_temp == "Error") return false;

		AtomicMap[contextStack.top()].pop_back();
		AtomicMapCnt--;

		addVar(it->second, contextStack.top(), "kwint");

		nextGraphState(0);
		generateString("AssignOrCall");

		if (!AssignOrCall()) return false;
	}

	else if (it->first == "id") {
		nextGraphState(0);
		generateString("AssignOrCall");

		if (!AssignOrCall()) return false;
	}

	if (it->first != "semicolon") return false;
	nextToken();

	rollbackGraphNode();
	return true;
}

FT LL::ForExp() {
	bool oneFlag = false;
	std::string _temp;

	if (it->first == "lpar" || it->first == "opdec" || it->first == "opinc" || it->first == "num" || it->first == "char" || it->first == "id") {
		nextGraphState(0);
		generateString("E");

		FT ERes = Expr();

		if (!ERes.first) oneFlag = true;
		else oneFlag = false;

		_temp = ERes.second;
	}

	if (it->first == "semicolon") {
		nextToken();

		rollbackGraphNode();
		if (oneFlag) return {true, "1"};
		return {true, _temp};
	}

	return {false, ""};
}

bool LL::ForLoop() {
	if (it->first == "opinc") {
		nextToken();

		if (it->first != "id") return false;
		std::string _temp = it->second;
		nextToken();

		auto p = checkVar(_temp);
		generateAtom(contextStack.top(), "ADD", p, "1", p);

		if (it->first != "rpar") return false;
		nextToken();

		nextGraphState(0);
		generateString("opinc " + _temp);

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	if (it->first == "opdec") {
		nextToken();

		if (it->first != "id") return false;
		std::string _temp = it->second;
		nextToken();

		auto p = checkVar(_temp);
		generateAtom(contextStack.top(), "SUB", p, "1", p);

		if (it->first != "rpar") return false;
		nextToken();

		nextGraphState(0);
		generateString("opdec " + _temp);

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	if (it->first == "id") {
		nextGraphState(0);
		generateString("AssignOrCall");

		if (!AssignOrCall()) return false;

		if (it->first != "rpar") return false;
		nextToken();

		rollbackGraphNode();
		return true;
	}

	if (it->first != "rpar") return false;
	nextToken();

	rollbackGraphNode();
	return true;
}

bool LL::IfOp() {
	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("lpar E");

	auto ERes = Expr();
	if (!ERes.first) return false;

	if (it->first != "rpar") return false;
	nextToken();

	auto l1 = newLabel();
	generateAtom(contextStack.top(), "EQ", ERes.second, "0", "L" + l1);

	nextGraphState(1);
	generateString("rpar Stmt");

	if (!Stmt()) return false;

	auto l2 = newLabel();
	generateAtom(contextStack.top(), "JMP", "", "", "L" + l2);
	generateAtom(contextStack.top(), "LBL", "", "", "L" + l1);

	nextGraphState(0);
	generateString("ElsePart");

	if (it->first == "kwelse") {
		if (!ElsePart()) return false;

		generateAtom(contextStack.top(), "LBL", "", "", "L" + l2);

		rollbackGraphNode();
		return true;
	}

	generateAtom(contextStack.top(), "LBL", "", "", "L" + l2);

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::ElsePart() {

	nextToken();

	nextGraphState(0);
	generateString("kwelse Stmt");

	if (!Stmt()) return false;

	rollbackGraphNode();
	return true;
}

bool LL::SwitchOp() {
	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("lpar E");

	auto ERes = Expr();
	if (!ERes.first) return false;

	if (it->first != "rpar") return false;
	nextToken();

	if (it->first != "lbrace") return false;
	nextToken();

	nextGraphState(1);
	generateString("rpar lbrace Cases");

	auto end = newLabel();
	if (!Cases(ERes.second, end)) return false;

	if (it->first != "rbrace") return false;
	nextToken();

	generateAtom(contextStack.top(), "LBL", "", "", "L" + end);

	nextGraphState(0);
	generateString("rbrace");

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::Cases(const std::string &p, const std::string &end) {
	nextGraphState(1);
	generateString("ACase");

	auto ACaseRes = ACase(p, end);
	if (!ACaseRes.first) return false;

	nextGraphState(0);
	generateString("Cases'");

	if (!CasesList(p, end, ACaseRes.second)) return false;

	rollbackGraphNode();
	return true;
}

bool LL::CasesList(const std::string &p, const std::string &end, const std::string &def) {

	if (it->first == "kwcase" || it->first == "kwdefault") {
		nextGraphState(1);
		generateString("ACase");

		FT ACaseRes = ACase(p, end);
		if (!ACaseRes.first) return false;

		if ((stoi(def) >= 0) && (stoi(ACaseRes.second) >= 0)) {
			generateAtom("SYSTEM", "ERROR", "ERROR", "ERROR", "ERROR");
		}

		auto maxDef = stoi(def) < stoi(ACaseRes.second) ? ACaseRes.second : def;

		nextGraphState(0);
		generateString("Cases'");

		if (!CasesList(p, end, maxDef)) return false;

		rollbackGraphNode();
		return true;
	}

	std::string q = stoi(def) >= 0 ? def : end;
	generateAtom(contextStack.top(), "JMP", "", "", "L" + q);

	rollbackGraphNode();
	return true;
}

FT LL::ACase(const std::string &p, const std::string &end) {
	if (it->first == "kwcase") {
		nextToken();

		if (it->first != "num") return {false, ""};
		std::string _temp = it->second;
		nextToken();

		std::string next = newLabel();
		generateAtom(contextStack.top(), "NE", p, _temp, "L" + next);

		if (it->first != "colon") return {false, ""};
		nextToken();

		nextGraphState(0);
		generateString("kwcase " + _temp + "colon StmtList");

		if (!StmtList()) return {false, ""};

		generateAtom(contextStack.top(), "JMP", "", "", "L" + end);
		generateAtom(contextStack.top(), "LBL", "", "", "L" + next);

		rollbackGraphNode();
		return {true, "-1"};
	}

	if (it->first == "kwdefault") {
		nextToken();

		if (it->first != "colon") return {false, ""};
		nextToken();

		auto next = newLabel();
		auto def = newLabel();

		generateAtom(contextStack.top(), "JMP", "", "", "L" + next);
		generateAtom(contextStack.top(), "LBL", "", "", "L" + def);

		nextGraphState(0);
		generateString("kwdefault colon StmtList");

		if (!StmtList()) return {false, ""};

		generateAtom(contextStack.top(), "JMP", "", "", "L" + end);
		generateAtom(contextStack.top(), "LBL", "", "", "L" + next);

		rollbackGraphNode();
		return {true, def};
	}

	return {false, ""};
}

bool LL::InOp() {
	if (it->first != "id") return false;
	std::string _temp = it->second;
	nextToken();

	if (it->first != "semicolon") return false;
	nextToken();

	auto p = checkVar(_temp);
	generateAtom(contextStack.top(), "IN", "", "", p);

	nextGraphState(0);
	generateString(_temp + " semicolon");

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::OutOp() {

	nextGraphState(1);
	generateString("OutOp'");

	if (!OutOpList()) return false;

	if (it->first != "semicolon") return false;
	nextToken();

	nextGraphState(0);
	generateString("semicolon");

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::OutOpList() {
	if (it->first == "str") {
		std::string _temp = it->second;
		nextToken();

		nextGraphState(0);
		generateString("\"" + _temp + "\"");

		generateAtom(contextStack.top(), "OUT", "", "", "\"" + _temp + "\"");
		rollbackGraphNode();
	} else {

		nextGraphState(0);
		generateString("E");

		auto ERes = Expr();
		if (!ERes.first) return false;

		generateAtom(contextStack.top(), "OUT", "", "", ERes.second);
	}

	rollbackGraphNode();
	return true;
}

FT LL::Type() {
	if (it->first == "eof") return {false, ""};

	if (it->first == "kwint" || it->first == "kwchar") {

		auto temp = it->first;
		nextToken();

		nextGraphState(0);
		generateString(temp);

		rollbackGraphNode();
		rollbackGraphNode();
		return {true, temp};
	}

	return {false, ""};
}

bool LL::DeclareVarList(const std::string &type) {
	if (it->first == "eof") return false;

	if (it->first == "comma") {
		nextToken();

		if (it->first != "id") return false;
		auto temp = it->second;

		nextGraphState(1);
		generateString("comma " + it->second + " InitVar");

		nextToken();

		if (!InitVar(type, temp)) return false;

		nextGraphState(0);
		generateString("DeclareVarList");

		if (!DeclareVarList(type)) return false;
	}

	rollbackGraphNode();
	return true;
}

bool LL::InitVar(const std::string &r, const std::string &s) {
	if (it->first == "eof") return false;

	if (it->first == "opassign") {
		nextToken();

		if (it->first == "num" || it->first == "char") {

			std::string newVar = "'" + addVar(s, contextStack.top(), r, it->second) + "'";

			if (newVar == "'Error'") return false;

			nextGraphState(0);
			generateString("opassign " + it->second);

			generateAtom(contextStack.top(), "MOV", it->second, "", newVar);

			nextToken();

			rollbackGraphNode();
			rollbackGraphNode();
			return true;
		}

		return false;
	}

	std::string _temp = addVar(s, contextStack.top(), r);

	if (_temp == "'Error'") return false;

	rollbackGraphNode();
	return true;
}

FT LL::ParamList() {
	if (it->first == "eof") return {false, ""};

	if (it->first == "kwint" || it->first == "kwchar") {
		nextGraphState(1);
		generateString("Type");

		auto TypeResult = Type();

		if (it->first != "id") return {false, ""};
		std::string _temp = addVar(it->second, contextStack.top(), TypeResult.second);

		if (_temp == "'Error'") return {false, ""};

		nextGraphState(0);
		generateString(it->second + " ParamList'");

		nextToken();

		FT ParLLRes = ParamListList();
		if (!ParLLRes.first) return {false, ""};

		rollbackGraphNode();
		return {true, std::to_string(stoi(ParLLRes.second) + 1)};
	}

	rollbackGraphNode();
	return {true, "0"};
}

FT LL::ParamListList() {
	if (it->first == "eof") return {false, ""};

	if (it->first == "comma") {
		nextToken();

		if (it->first == "kwint" || it->first == "kwchar") {
			nextGraphState(1);
			generateString("comma Type");

			auto TypeResult = Type();

			if (it->first != "id") return {false, ""};
			std::string _temp = addVar(it->second, contextStack.top(), TypeResult.second);

			if (_temp == "'Error'") return {false, ""};

			nextGraphState(0);
			generateString(it->second + " ParamList'");

			nextToken();

			FT ParLLRes = ParamListList();
			if (!ParLLRes.first) return {false, ""};

			rollbackGraphNode();
			return {true, std::to_string(stoi(ParLLRes.second) + 1)};
		}
	}

	rollbackGraphNode();
	return {true, "0"};
}
