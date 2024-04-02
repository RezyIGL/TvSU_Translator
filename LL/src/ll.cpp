#include "ll.h"

LL::LL(std::istream &stream) : lexer {stream} {}

LL::~LL() = default;

std::string LL::validate() {
	Token token;
	while (token.first != "eof") {
		token = lexer.getNextToken();
		tempVec.emplace_back(token);
	}

	it = tempVec.begin();
	currentTokenValue = it->first;

	if (Expr()) {
		return "Accepted!";
	}

	return "Incorrect Expression!";
}

void LL::nextToken() {
	it++;
	currentTokenValue = it->first;
}

bool LL::Expr() {
	if (!Expr7()) return false;
	return true;
};

bool LL::Expr7() {
	if (!Expr6()) return false;
	if (!Expr7List()) return false;
	return true;
};

bool LL::Expr7List() {
	if (it >= tempVec.end()) return true;

	if (currentTokenValue == "opor") {
		nextToken();
		if (!Expr6()) return false;
		if (!Expr7List()) return false;
	}

	return true;
};

bool LL::Expr6() {
	if (!Expr5()) return false;
	if (!Expr6List()) return false;
	return true;
};

bool LL::Expr6List() {
	if (it >= tempVec.end()) return true;

	if (currentTokenValue == "opand") {
		nextToken();
		if (!Expr5()) return false;
		if (!Expr6List()) return false;
	}

	return true;
};

bool LL::Expr5() {
	if (!Expr4()) return false;
	if (!Expr5List()) return false;
	return true;
};

bool LL::Expr5List() {
	if (it >= tempVec.end()) return true;

	if (currentTokenValue == "opeq") {
		nextToken();
		if (!Expr4()) return false;
	} else if (currentTokenValue == "opne") {
		nextToken();
		if (!Expr4()) return false;
	} else if (currentTokenValue == "ople") {
		nextToken();
		if (!Expr4()) return false;
	}

	return true;
};

bool LL::Expr4() {
	if (!Expr3()) return false;
	if (!Expr4List()) return false;
	return true;
};

bool LL::Expr4List() {
	if (it >= tempVec.end()) return true;

	if (currentTokenValue == "opplus") {
		nextToken();
		if (!Expr3()) return false;
		if (!Expr4List()) return false;
	} else if (currentTokenValue == "opminus") {
		nextToken();
		if (!Expr3()) return false;
		if (!Expr4List()) return false;
	}

	return true;
};

bool LL::Expr3() {
	if (!Expr2()) return false;
	if (!Expr3List()) return false;
	return true;
};

bool LL::Expr3List() {
	if (it >= tempVec.end()) return true;

	if (currentTokenValue == "opmul") {
		nextToken();
		if (!Expr3()) return false;
		if (!Expr4List()) return false;
	}

	return true;
};

bool LL::Expr2() {
	if (it >= tempVec.end()) return true;

	if (currentTokenValue == "opnot") {
		nextToken();
	}

	if (!Expr1()) return false;

	return true;
};

bool LL::Expr1() {
	if (it >= tempVec.end()) return false;

	if (currentTokenValue == "opinc") {
		nextToken();
		if (currentTokenValue != "id") return false;
		nextToken();
		return true;
	} else if (currentTokenValue == "lpar") {
		nextToken();
		if (!Expr()) return false;
		if (currentTokenValue != "rpar") return false;
		nextToken();
		return true;
	} else if (currentTokenValue == "num") {
		nextToken();
		return true;
	} else if (currentTokenValue == "id") {
		nextToken();
		return true;
	}

	return false;
};
