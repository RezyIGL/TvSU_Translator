#ifndef LEXER_SYM_H
#define LEXER_SYM_H

#include "Lexer.h"
#include <stack>
#include <queue>

class ShuntingYard {
public:
	ShuntingYard(std::istream&);
	virtual ~ShuntingYard();
	Lexem getNextLexem();

private:
	Lexer lexer;
	std::stack<Lexem> myStack;
	std::queue<Lexem> output;
	bool expectOperand = true;
	void lexemOperand(const Lexem&);
	void lexemLeftParentheses(const Lexem&);
	void lexemRightParentheses();
	void similarSign(const Lexem&);
	void higherPriority(const Lexem&);
	void lexemPlusOrMinus(const Lexem&);
	void lexemMultiply(const Lexem&);
	void lexemEOFOrError(const Lexem&);
	void processLexemInput(const Lexem&);
};

#endif //LEXER_SYM_H
