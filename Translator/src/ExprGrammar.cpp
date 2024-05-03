#include "Translator.h"

// Expressions check
FT LL::Expr() {

	nextGraphState(0);
	generateString("E7");

	auto E7Result = Expr7();
	if (!E7Result.first) return {false, ""};

	rollbackGraphNode();
	return {true, E7Result.second};
}

FT LL::Expr7() {

	nextGraphState(1);
	generateString("E6");

	auto E6Result = Expr6();
	if (!E6Result.first) return {false, ""};

	nextGraphState(0);
	generateString("E7'");

	auto E7ListResult = Expr7List(E6Result.second);
	if (!E7ListResult.first) return {false, ""};

	rollbackGraphNode();
	return {true, E7ListResult.second};
}

FT LL::Expr7List(const std::string &funcID) {
	if (it->first == "opor") {
		nextToken();

		nextGraphState(1);
		generateString("opor E6");

		auto E6Result = Expr6();
		if (!E6Result.first) return {false, ""};

		auto s = alloc(contextStack.top());
		generateAtom(contextStack.top(), "OR", funcID, E6Result.second, s);

		nextGraphState(0);
		generateString("E7'");

		auto E7ListResult = Expr7List(s);
		if (!E7ListResult.first) return {false, ""};

		rollbackGraphNode();
		return {true, E7ListResult.second};
	}

	rollbackGraphNode();
	return {true, funcID};
}

FT LL::Expr6() {
	nextGraphState(1);
	generateString("E5");

	auto E5Result = Expr5();
	if (!E5Result.first) return {false, ""};

	nextGraphState(0);
	generateString("E6'");

	auto E6ListResult = Expr6List(E5Result.second);
	if (!E6ListResult.first) return {false, ""};

	rollbackGraphNode();
	return {true, E6ListResult.second};
}

FT LL::Expr6List(const std::string &funcID) {
	if (it->first == "opand") {
		nextToken();

		nextGraphState(1);
		generateString("opand E5");

		auto E5Result = Expr5();
		if (!E5Result.first) return {false, ""};

		auto s = alloc(contextStack.top());
		generateAtom(contextStack.top(), "AND", funcID, E5Result.second, s);

		nextGraphState(0);
		generateString("E6'");

		auto E6ListResult = Expr6List(s);
		if (!E6ListResult.first) return {false, ""};

		rollbackGraphNode();
		return {true, E6ListResult.second};
	}

	rollbackGraphNode();
	return {true, funcID};
}

FT LL::Expr5() {

	nextGraphState(1);
	generateString("E4");

	auto E4Result = Expr4();
	if (!E4Result.first) return {false, ""};

	nextGraphState(0);
	generateString("E5'");

	auto E5ListResult = Expr5List(E4Result.second);
	if (!E5ListResult.first) return {false, ""};

	rollbackGraphNode();
	return {true, E5ListResult.second};
}

FT LL::Expr5List(const std::string &funcID) {
	if (it->first == "opeq") {
		nextToken();

		nextGraphState(0);
		generateString("opeq E4");

		auto E4Result = Expr4();
		if (!E4Result.first) return {false, ""};

		auto s = alloc(contextStack.top());
		auto l = newLabel();

		generateAtom(contextStack.top(), "MOV", "'1'", "", s);
		generateAtom(contextStack.top(), "EQ", funcID, E4Result.second, "L" + l);
		generateAtom(contextStack.top(), "MOV", "'0'", "", s);
		generateAtom(contextStack.top(), "LBL", "", "", "L" + l);

		rollbackGraphNode();
		return {true, s};

	} else if (it->first == "opne") {
		nextToken();

		nextGraphState(0);
		generateString("opne E4");

		auto E4Result = Expr4();
		if (!E4Result.first) return {false, ""};

		auto s = alloc(contextStack.top());
		auto l = newLabel();

		generateAtom(contextStack.top(), "MOV", "'1'", "", s);
		generateAtom(contextStack.top(), "NE", funcID, E4Result.second, "L" + l);
		generateAtom(contextStack.top(), "MOV", "'0'", "", s);
		generateAtom(contextStack.top(), "LBL", "", "", "L" + l);

		rollbackGraphNode();
		return {true, s};
	} else if (it->first == "ople") {
		nextToken();

		nextGraphState(0);
		generateString("ople E4");

		auto E4Result = Expr4();
		if (!E4Result.first) return {false, ""};

		auto s = alloc(contextStack.top());
		auto l = newLabel();

		generateAtom(contextStack.top(), "MOV", "'1'", "", s);
		generateAtom(contextStack.top(), "LE", funcID, E4Result.second, "L" + l);
		generateAtom(contextStack.top(), "MOV", "'0'", "", s);
		generateAtom(contextStack.top(), "LBL", "", "", "L" + l);

		rollbackGraphNode();
		return {true, s};
	} else if (it->first == "opgt") {
		nextToken();

		nextGraphState(0);
		generateString("opgt E4");

		auto E4Result = Expr4();
		if (!E4Result.first) return {false, ""};

		auto s = alloc(contextStack.top());
		auto l = newLabel();

		generateAtom(contextStack.top(), "MOV", "'1'", "", s);
		generateAtom(contextStack.top(), "GT", funcID, E4Result.second, "L" + l);
		generateAtom(contextStack.top(), "MOV", "'0'", "", s);
		generateAtom(contextStack.top(), "LBL", "", "", "L" + l);

		rollbackGraphNode();
		return {true, s};
	} else if (it->first == "opge") {
		nextToken();

		nextGraphState(0);
		generateString("opge E4");

		auto E4Result = Expr4();
		if (!E4Result.first) return {false, ""};

		auto s = alloc(contextStack.top());
		auto l = newLabel();

		generateAtom(contextStack.top(), "MOV", "'1'", "", s);
		generateAtom(contextStack.top(), "GE", funcID, E4Result.second, "L" + l);
		generateAtom(contextStack.top(), "MOV", "'0'", "", s);
		generateAtom(contextStack.top(), "LBL", "", "", "L" + l);

		rollbackGraphNode();
		return {true, s};
	} else if (it->first == "oplt") {
		nextToken();

		nextGraphState(0);
		generateString("oplt E4");

		auto E4Result = Expr4();
		if (!E4Result.first) return {false, ""};

		auto s = alloc(contextStack.top());
		auto l = newLabel();

		generateAtom(contextStack.top(), "MOV", "'1'", "", s);
		generateAtom(contextStack.top(), "LT", funcID, E4Result.second, "L" + l);
		generateAtom(contextStack.top(), "MOV", "'0'", "", s);
		generateAtom(contextStack.top(), "LBL", "", "", "L" + l);

		rollbackGraphNode();
		return {true, s};
	}

	rollbackGraphNode();
	return {true, funcID};
}

FT LL::Expr4() {

	nextGraphState(1);
	generateString("E3");

	auto E3Result = Expr3();
	if (!E3Result.first) return {false, ""};

	nextGraphState(0);
	generateString("E4'");

	auto E4ListResult = Expr4List(E3Result.second);
	if (!E4ListResult.first) return {false, ""};

	rollbackGraphNode();
	return {true, E4ListResult.second};
}

FT LL::Expr4List(const std::string &funcID) {
	if (it->first == "opplus") {
		nextToken();

		nextGraphState(1);
		generateString("opplus E3");

		auto E3Result = Expr3();
		if (!E3Result.first) return {false, ""};

		auto s = alloc(contextStack.top());
		generateAtom(contextStack.top(), "ADD", funcID, E3Result.second, s);

		nextGraphState(0);
		generateString("E4'");

		auto E4ListResult = Expr4List(s);
		if (!E4ListResult.first) return {false, ""};

		rollbackGraphNode();
		return {true, E4ListResult.second};

	} else if (it->first == "opminus") {
		nextToken();

		nextGraphState(1);
		generateString("opminus E3");

		auto E3Result = Expr3();
		if (!E3Result.first) return {false, ""};

		auto s = alloc(contextStack.top());
		generateAtom(contextStack.top(), "SUB", funcID, E3Result.second, s);

		nextGraphState(0);
		generateString("E4'");

		auto E4ListResult = Expr4List(s);
		if (!E4ListResult.first) return {false, ""};

		rollbackGraphNode();
		return {true, E4ListResult.second};
	}

	rollbackGraphNode();
	return {true, funcID};
}

FT LL::Expr3() {

	nextGraphState(1);
	generateString("E2");

	auto E2Result = Expr2();
	if (!E2Result.first) return {false, ""};

	nextGraphState(0);
	generateString("E3'");

	auto E3ListResult = Expr3List(E2Result.second);
	if (!E3ListResult.first) return {false, ""};

	rollbackGraphNode();
	return {true, E3ListResult.second};

}

FT LL::Expr3List(const std::string &funcID) {
	if (it->first == "opmul") {
		nextToken();

		nextGraphState(1);
		generateString("opmul E2");

		auto E2Result = Expr2();
		if (!E2Result.first) return {false, ""};

		auto s = alloc(contextStack.top());
		generateAtom(contextStack.top(), "MUL", funcID, E2Result.second, s);

		nextGraphState(0);
		generateString("E3'");

		auto E3ListResult = Expr3List(s);
		if (!E3ListResult.first) return {false, ""};

		rollbackGraphNode();
		return {true, E3ListResult.second};
	}

	rollbackGraphNode();
	return {true, funcID};
}

FT LL::Expr2() {
	if (it->first == "opnot") {
		nextToken();

		nextGraphState(0);
		generateString("opnot E1");

		auto E1Result = Expr1();
		if (!E1Result.first) return {false, ""};

		auto r = alloc(contextStack.top());
		generateAtom(contextStack.top(), "NOT", E1Result.second, "", r);

		rollbackGraphNode();
		return {true, r};
	} else {

		nextGraphState(0);
		generateString("E1");

		auto E1Result = Expr1();
		if (!E1Result.first) return {false, ""};

		rollbackGraphNode();
		return {true, E1Result.second};
	}
}

FT LL::Expr1() {
	if (it->first == "opdec") {
		nextToken();

		if (it->first != "id") return {false, ""};

		auto q = checkVar(it->second);
		generateAtom(contextStack.top(), "SUB", q, "'1'", q);

		nextGraphState(0);
		generateString("opdec " + it->second);
		nextToken();

		rollbackGraphNode();
		rollbackGraphNode();
		return {true, q};
	} else if (it->first == "opinc") {
		nextToken();

		if (it->first != "id") return {false, ""};

		auto q = checkVar(it->second);
		generateAtom(contextStack.top(), "ADD", q, "'1'", q);

		nextGraphState(0);
		generateString("opinc " + it->second);
		nextToken();

		rollbackGraphNode();
		rollbackGraphNode();
		return {true, q};
	} else if (it->first == "lpar") {
		nextToken();

		nextGraphState(1);
		generateString("lpar E");

		auto EResult = Expr();
		if (!EResult.first) return {false, ""};

		nextGraphState(0);
		generateString("rpar");

		if (it->first != "rpar") return {false, ""};
		nextToken();

		rollbackGraphNode();
		rollbackGraphNode();
		return {true, EResult.second};
	} else if (it->first == "num") {
		auto val = it->second;

		nextGraphState(0);
		generateString(val);
		nextToken();

		rollbackGraphNode();
		rollbackGraphNode();
		return {true, "'" + val + "'"};
	} else if (it->first == "char") {
		auto val = it->second;

		nextGraphState(0);
		generateString(val);
		nextToken();

		rollbackGraphNode();
		rollbackGraphNode();
		return {true, val};
	} else if (it->first == "id") {
		auto r = it->second;

		nextGraphState(0);
		generateString(r + " E1'");

		nextToken();

		auto E1ListResult = Expr1List(r);
		if (!E1ListResult.first) return {false, ""};

		rollbackGraphNode();
		return {true, E1ListResult.second};
	}

	return {false, ""};
}

FT LL::Expr1List(const std::string &funcID) {
	if (it->first == "lpar") {
		nextToken();

		nextGraphState(1);
		generateString("lpar ArgList");

		auto ArgListResult = ArgList();
		if (!ArgListResult.first) return {false, ""};

		nextGraphState(0);
		generateString("rpar");

		if (it->first != "rpar") return {false, ""};
		nextToken();

		auto s = checkFunc(funcID, ArgListResult.second);
		auto r = alloc(contextStack.top());

		generateAtom(contextStack.top(), "CALL", s, "", r);

		rollbackGraphNode();
		rollbackGraphNode();
		return {true, r};
	}

	if (it->first == "opinc") {
		nextToken();

		auto s = checkVar(funcID);
		auto r = alloc(contextStack.top());

		generateAtom(contextStack.top(), "MOV", s, "", r);
		generateAtom(contextStack.top(), "ADD", s, "'1'", s);

		nextGraphState(0);
		generateString("opinc");
		rollbackGraphNode();
		rollbackGraphNode();
		return {true, r};
	}

	if (it->first == "opdec") {
		nextToken();

		auto s = checkVar(funcID);
		auto r = alloc(contextStack.top());

		generateAtom(contextStack.top(), "MOV", s, "", r);
		generateAtom(contextStack.top(), "SUB", s, "'1'", s);

		nextGraphState(0);
		generateString("opdec");
		rollbackGraphNode();
		rollbackGraphNode();
		return {true, r};
	}

	auto q = checkVar(funcID);

	rollbackGraphNode();
	return {true, q};
}

FT LL::ArgList() {
	if (it->first == "lpar" || it->first == "opdec" || it->first == "opinc" || it->first == "num" || it->first == "char" || it->first == "id") {

		nextGraphState(1);
		generateString(it->first + " E");

		FT ERes = Expr();
		if (!ERes.first) return {false, ""};

        generateAtom(contextStack.top(), "PARAM", "", "", ERes.second);

		nextGraphState(0);
		generateString("ArgList'");

		FT ArgListListResult = ArgListList();
		if (!ArgListListResult.first) return {false, ""};

		rollbackGraphNode();
		return {true, std::to_string(stoi(ArgListListResult.second) + 1)};
	}

	rollbackGraphNode();
	return {true, "0"};
}

FT LL::ArgListList() {
	if (it->first == "comma") {
		nextToken();

		if (it->first == "lpar" || it->first == "opdec" || it->first == "opinc" || it->first == "num" || it->first == "char" ||
		    it->first == "id") {

			nextGraphState(1);
			generateString("comma E");

			auto ERes = Expr();
			if (!ERes.first) return {false, ""};

            generateAtom(contextStack.top(), "PARAM", "", "", ERes.second);

			nextGraphState(0);
			generateString("ArgList'");

			FT ArgListListResult = ArgListList();
			if (!ArgListListResult.first) return {false, ""};

			rollbackGraphNode();
			return {true, std::to_string(stoi(ArgListListResult.second) + 1)};
		} else {
			return {false, ""};
		}
	}

	rollbackGraphNode();
	return {true, "0"};
}
