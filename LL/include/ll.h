#ifndef TVSU_TRANSLATOR_LL_H
#define TVSU_TRANSLATOR_LL_H

#include "Lexer.h"

class LL {
public:
	explicit LL(std::istream&);
	virtual ~LL();
	std::string validate();
private:
	Lexer lexer;
	std::vector<Token> tempVec;
	bool Expr();
	bool Expr7();
	bool Expr7List();
	bool Expr6();
	bool Expr6List();
	bool Expr5();
	bool Expr5List();
	bool Expr4();
	bool Expr4List();
	bool Expr3();
	bool Expr3List();
	bool Expr2();
	bool Expr1();
	bool Expr1List();
	std::vector<Token>::iterator it;
	std::string currentTokenValue;
	void nextToken();
};


#endif //TVSU_TRANSLATOR_LL_H
