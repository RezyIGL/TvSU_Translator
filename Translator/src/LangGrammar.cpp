#include "Translator.h"

// MiniC grammar check
bool LL::StmtList(const std::string &context) {
	if (it->first == "eof") return true;

	auto _temp = it;

	nextGraphState(1);
	generateString("Stmt");

	bool result = Stmt(context);
	if (!result) {
		outputVector.pop_back();

		it = _temp;

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	nextGraphState(0);
	generateString("StmtList");

	bool tailResult = StmtList(context);
	if (!tailResult) return false;

	rollbackGraphNode();
	return true;
}

bool LL::Stmt(const std::string &context) {
	if (it->first == "eof") return false;

	if (it->first == "kwint" || it->first == "kwchar") {
		nextGraphState(0);
		generateString("DeclareStmt");

		if (!DeclareStmt(context)) return false;

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

	if (context == "-1") return false;

	if (it->first == "id") {
		nextGraphState(0);
		generateString("AssignOrCallOp");

		if (!AssignOrCallOp(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwwhile") {
		nextToken();

		nextGraphState(0);
		generateString("kwwhile WhileOp");

		if (!WhileOp(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwfor") {
		nextToken();

		nextGraphState(0);
		generateString("kwfor ForOp");

		if (!ForOp(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwif") {
		nextToken();

		nextGraphState(0);
		generateString("kwif IfOp");

		if (!IfOp(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwswitch") {
		nextToken();

		nextGraphState(0);
		generateString("kwswitch SwitchOp");

		if (!SwitchOp(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwin") {
		nextToken();

		nextGraphState(0);
		generateString("kwin InOp");

		if (!InOp(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "kwout") {
		nextToken();

		nextGraphState(0);
		generateString("kwout OutOp");

		if (!OutOp(context)) return false;

		rollbackGraphNode();
		return true;
	}

	if (it->first == "lbrace") {
		nextToken();

		nextGraphState(1);
		generateString("lbrace StmtList");

		bool result = StmtList(context);
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

		auto EResult = Expr(context);
		if (!EResult.first) return false;

		generateAtom(context, "RET", "", "", EResult.second);


		if (it->first != "semicolon") return false;
		nextToken();

		nextGraphState(0);
		generateString("semicolon");

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	return false;
}

bool LL::DeclareStmt(const std::string &context) {

	nextGraphState(1);
	generateString("Type");

	auto TypeResult = Type(context);
	if (!TypeResult.first) return false;

	if (it->first != "id") return false;
	auto temp = it->second;
	nextToken();

	nextGraphState(0);
	generateString(temp + " DeclareStmt'");

	if (!DeclareStmtList(context, TypeResult.second, temp)) return false;

	rollbackGraphNode();
	return true;
}

bool LL::DeclareStmtList(const std::string &context, const std::string &type, const std::string &name) {
	if (it->first == "eof") return false;

	if (it->first == "lpar") {
		if (stoi(context) > -1) return false;
		nextToken();

		nextGraphState(1);
		generateString("lpar ParamList");

		std::string TC = addFunc(name, type);

		auto ParamListRes = ParamList(TC);
		if (!ParamListRes.first) return false;

		for (auto &i : AtomicMap["-1"]) {
			if (i.cnt == stoi(TC)) {
				i.init = ParamListRes.second;
			}
		}

		if (it->first != "rpar") return false;
		nextToken();

		if (it->first != "lbrace") return false;
		nextToken();

		nextGraphState(1);
		generateString("rpar lbrace StmtList");

		if (!StmtList(TC)) return false;

		if (it->first != "rbrace") return false;
		nextToken();

		generateAtom(context, "RET", "", "", "0");

		nextGraphState(0);
		generateString("rbrace");

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	} else if (it->first == "opassign") {
		nextToken();
		auto _temp = it->second;

		if (it->first == "num") {
			addVar(name, context, type, _temp);
			nextToken();

			nextGraphState(1);
			generateString("opassign " + _temp + " DeclareVarList");

			auto DecVarListRes = DeclareVarList(context, type);
			if (!DecVarListRes) return false;

			if (it->first != "semicolon") return false;
			nextToken();

			nextGraphState(0);
			generateString("semicolon");

			rollbackGraphNode();
			rollbackGraphNode();
			return true;
		} else if (it->first == "char") {
			addVar(name, context, type, _temp);
			nextToken();

			nextGraphState(1);
			generateString("opassign " + _temp + "DeclareVarList");

			auto DecVarListRes = DeclareVarList(context, type);
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
		addVar(name, context, type);

		nextGraphState(1);
		generateString("DeclareVarList");

		if (!DeclareVarList(context, type)) return false;

		if (it->first != "semicolon") return false;
		nextToken();

		nextGraphState(0);
		generateString("semicolon");

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}
}

bool LL::AssignOrCallOp(const std::string &context) {

	nextGraphState(1);
	generateString("AssignOrCall");

	if (!AssignOrCall(context)) return false;

	if (it->first != "semicolon") return false;
	nextToken();

	nextGraphState(0);
	generateString("semicolon");

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::AssignOrCall(const std::string &context) {
	if (it->first != "id") return false;
	auto temp = it->second;
	nextToken();

	nextGraphState(0);
	generateString(temp + " AssignOrCall'");

	if (!AssignOrCallList(context, temp)) return false;

	rollbackGraphNode();
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

		rollbackGraphNode();
		return true;
	}

	if (it->first == "lpar") {
		nextToken();

		nextGraphState(1);
		generateString("lpar ArgList");

		auto ArgListRes = ArgList(context);
		if (!ArgListRes.first) return false;

		if (it->first != "rpar") return false;
		nextToken();

		nextGraphState(0);
		generateString("rpar");

		auto q = checkFunc(name, ArgListRes.second);
		auto r = alloc(context);
		generateAtom(context, "CALL", q, "", r);

		rollbackGraphNode();
		rollbackGraphNode();
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

	nextGraphState(1);
	generateString("lpar E");

	auto ERes = Expr(context);
	if (!ERes.first) return false;

	generateAtom(context, "EQ", ERes.second, "0", "L" + l2);

	if (it->first != "rpar") return false;
	nextToken();

	nextGraphState(0);
	generateString("rpar Stmt");

	if (!Stmt(context)) return false;

	generateAtom(context, "JMP", "", "", "L" + l1);
	generateAtom(context, "LBL", "", "", "L" + l2);

	rollbackGraphNode();
	return true;
}

bool LL::ForOp(const std::string &context) {

	auto l1 = newLabel();
	auto l2 = newLabel();
	auto l3 = newLabel();
	auto l4 = newLabel();

	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("lpar ForInit");

	if (!ForInit(context)) return false;

	generateAtom(context, "LBL", "", "", "L" + l1);

	nextGraphState(1);
	generateString("semicolon ForExp");

	FT ForExpRes = ForExp(context);
	if (!ForExpRes.first) return false;

	generateAtom(context, "EQ", ForExpRes.second, "0", "L" + l4);
	generateAtom(context, "JMP", "", "", "L" + l3);
	generateAtom(context, "LBL", "", "", "L" + l2);

	nextGraphState(1);
	generateString("semicolon ForLoop");

	if (!ForLoop(context)) return false;

	generateAtom(context, "JMP", "", "", "L" + l1);
	generateAtom(context, "LBL", "", "", "L" + l3);

	nextGraphState(0);
	generateString("rpar Stmt");

	if (!Stmt(context)) return false;

	generateAtom(context, "JMP", "", "", "L" + l2);
	generateAtom(context, "LBL", "", "", "L" + l4);

	rollbackGraphNode();
	return true;
}

bool LL::ForInit(const std::string &context) {
	if (it->first == "id") {
		nextGraphState(0);
		generateString("AssignOrCall");

		if (!AssignOrCall(context)) return false;
	}

	if (it->first != "semicolon") return false;
	nextToken();

	rollbackGraphNode();
	return true;
}

FT LL::ForExp(const std::string &context) {
	bool oneFlag = false;
	std::string _temp;

	if (it->first == "lpar" || it->first == "opinc" || it->first == "num" || it->first == "char" || it->first == "id") {
		nextGraphState(0);
		generateString("E");

		FT ERes = Expr(context);

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

		nextGraphState(0);
		generateString("opinc " + _temp);

		rollbackGraphNode();
		rollbackGraphNode();
		return true;
	}

	if (it->first == "id") {
		nextGraphState(1);
		generateString("AssignOrCall");

		if (!AssignOrCall(context)) return false;

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

bool LL::IfOp(const std::string &context) {
	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("lpar E");

	auto ERes = Expr(context);
	if (!ERes.first) return false;

	if (it->first != "rpar") return false;
	nextToken();

	auto l1 = newLabel();
	generateAtom(context, "EQ", ERes.second, "0", "L" + l1);

	nextGraphState(1);
	generateString("rpar Stmt");

	if (!Stmt(context)) return false;

	auto l2 = newLabel();
	generateAtom(context, "JMP", "", "", "L" + l2);
	generateAtom(context, "LBL", "", "", "L" + l1);

	nextGraphState(0);
	generateString("ElsePart");

	if (it->first == "kwelse") {
		if (!ElsePart(context)) return false;

		generateAtom(context, "LBL", "", "", "L" + l2);

		rollbackGraphNode();
		return true;
	}

	generateAtom(context, "LBL", "", "", "L" + l2);

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::ElsePart(const std::string &context) {

	nextToken();

	nextGraphState(0);
	generateString("kwelse Stmt");

	if (!Stmt(context)) return false;

	rollbackGraphNode();
	return true;
}

bool LL::SwitchOp(const std::string &context) {
	if (it->first != "lpar") return false;
	nextToken();

	nextGraphState(1);
	generateString("lpar E");

	auto ERes = Expr(context);
	if (!ERes.first) return false;

	if (it->first != "rpar") return false;
	nextToken();

	if (it->first != "lbrace") return false;
	nextToken();

	nextGraphState(1);
	generateString("rpar lbrace Cases");

	auto end = newLabel();
	if (!Cases(context, ERes.second, end)) return false;

	if (it->first != "rbrace") return false;
	nextToken();

	generateAtom(context, "LBL", "", "", "L" + end);

	nextGraphState(0);
	generateString("rbrace");

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::Cases(const std::string &context, const std::string &p, const std::string &end) {
	nextGraphState(1);
	generateString("ACase");

	auto ACaseRes = ACase(context, p, end);
	if (!ACaseRes.first) return false;

	nextGraphState(0);
	generateString("Cases'");

	if (!CasesList(context, p, end, ACaseRes.second)) return false;

	rollbackGraphNode();
	return true;
}

bool LL::CasesList(const std::string &context, const std::string &p, const std::string &end, const std::string &def) {

	if (it->first == "kwcase" || it->first == "kwdefault") {
		nextGraphState(1);
		generateString("ACase");

		FT ACaseRes = ACase(context, p, end);
		if (!ACaseRes.first) return false;

		if ((stoi(def) >= 0) && (stoi(ACaseRes.second) >= 0)) {
			generateAtom("SYSTEM", "ERROR", "ERROR", "ERROR", "ERROR");
		}

		auto maxDef = stoi(def) < stoi(ACaseRes.second) ? ACaseRes.second : def;

		nextGraphState(0);
		generateString("Cases'");

		if (!CasesList(context, p, end, maxDef)) return false;

		rollbackGraphNode();
		return true;
	}

	std::string q = stoi(def) >= 0 ? def : end;
	generateAtom(context, "JMP", "", "", "L" + q);

	rollbackGraphNode();
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

		nextGraphState(0);
		generateString("kwcase " + _temp + "colon StmtList");

		if (!StmtList(context)) return {false, ""};

		generateAtom(context, "JMP", "", "", "L" + end);
		generateAtom(context, "LBL", "", "", "L" + next);

		rollbackGraphNode();
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

		nextGraphState(0);
		generateString("kwdefault colon StmtList");

		if (!StmtList(context)) return {false, ""};

		generateAtom(context, "JMP", "", "", "L" + end);
		generateAtom(context, "LBL", "", "", "L" + next);

		rollbackGraphNode();
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

	nextGraphState(0);
	generateString(_temp + " semicolon");

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::OutOp(const std::string &context) {

	nextGraphState(1);
	generateString("OutOp'");

	if (!OutOpList(context)) return false;

	if (it->first != "semicolon") return false;
	nextToken();

	nextGraphState(0);
	generateString("semicolon");

	rollbackGraphNode();
	rollbackGraphNode();
	return true;
}

bool LL::OutOpList(const std::string &context) {
	if (it->first == "str") {
		std::string _temp = it->second;
		nextToken();

		nextGraphState(0);
		generateString("\"" + _temp + "\"");

		generateAtom(context, "OUT", "", "", "\"" + _temp + "\"");
		rollbackGraphNode();
	} else {

		nextGraphState(0);
		generateString("E");

		auto ERes = Expr(context);
		if (!ERes.first) return false;

		generateAtom(context, "OUT", "", "", ERes.second);
	}

	rollbackGraphNode();
	return true;
}

FT LL::Type(const std::string &context) {
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

bool LL::DeclareVarList(const std::string &context, const std::string &type) {
	if (it->first == "eof") return false;

	if (it->first == "comma") {
		nextToken();

		if (it->first != "id") return false;
		auto temp = it->second;

		nextGraphState(1);
		generateString("comma " + it->second + " InitVar");

		nextToken();

		if (!InitVar(context, type, temp)) return false;

		nextGraphState(0);
		generateString("DeclareVarList");

		if (!DeclareVarList(context, type)) return false;
	}

	rollbackGraphNode();
	return true;
}

bool LL::InitVar(const std::string &context, const std::string &r, const std::string &s) {
	if (it->first == "eof") return false;

	if (it->first == "opassign") {
		nextToken();

		if (it->first == "num" || it->first == "char") {

			addVar(s, context, r, it->second);

			nextGraphState(0);
			generateString("opassign " + it->second);

			nextToken();

			rollbackGraphNode();
			rollbackGraphNode();
			return true;
		}

		return false;
	}

	addVar(s, context, r);
	rollbackGraphNode();
	return true;
}

FT LL::ParamList(const std::string &context) {
	if (it->first == "eof") return {false, ""};

	if (it->first == "kwint" || it->first == "kwchar") {
		nextGraphState(1);
		generateString("Type");

		auto TypeResult = Type(context);

		if (it->first != "id") return {false, ""};
		addVar(it->second, context, TypeResult.second);

		nextGraphState(0);
		generateString(it->second + " ParamList'");

		nextToken();

		FT ParLLRes = ParamListList(context);
		if (!ParLLRes.first) return {false, ""};

		rollbackGraphNode();
		return {true, std::to_string(stoi(ParLLRes.second) + 1)};
	}

	rollbackGraphNode();
	return {true, "0"};
}

FT LL::ParamListList(const std::string &context) {
	if (it->first == "eof") return {false, ""};

	if (it->first == "comma") {
		nextToken();

		if (it->first == "kwint" || it->first == "kwchar") {
			nextGraphState(1);
			generateString("comma Type");

			auto TypeResult = Type(context);

			if (it->first != "id") return {false, ""};
			addVar(it->second, context, TypeResult.second);

			nextGraphState(0);
			generateString(it->second + " ParamList'");

			nextToken();

			FT ParLLRes = ParamListList(context);
			if (!ParLLRes.first) return {false, ""};

			rollbackGraphNode();
			return {true, std::to_string(stoi(ParLLRes.second) + 1)};
		}
	}

	rollbackGraphNode();
	return {true, "0"};
}
