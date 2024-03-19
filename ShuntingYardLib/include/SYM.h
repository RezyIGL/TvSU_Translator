#ifndef LEXER_SYM_H
#define LEXER_SYM_H

#include "Lexer.h"
#include <stack>
#include <queue>

class ShuntingYard {
public:
	explicit ShuntingYard(std::istream&);
	virtual ~ShuntingYard();
	Token getNextToken();

private:
	Lexer lexer;
	std::stack<Token> myStack;
	std::queue<Token> output;
	bool expectOperand = true;
	void TokenOperand(const Token&);
	void TokenLeftParentheses(const Token&);
	void TokenRightParentheses();
	void similarSign(const Token&);
	void higherPriority(const Token&);
	void TokenPlusOrMinus(const Token&);
	void TokenMultiply(const Token&);
	void TokenEOFOrError(const Token&);
	void processTokenInput(const Token&);
};

#endif //LEXER_SYM_H
