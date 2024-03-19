#include "SYM.h"
#include <iostream>

ShuntingYard::ShuntingYard(std::istream &stream) : lexer { stream } {}

void ShuntingYard::TokenOperand(const Token &Token) {
	if (expectOperand) {
		output.push(Token);
		expectOperand = !expectOperand;
	} else {
		output.emplace("error", "");
	}
}

void ShuntingYard::TokenLeftParentheses(const Token &Token) {
	if (expectOperand) {
		myStack.emplace(Token);
	} else {
		output.emplace("error", "");
	}
}

void ShuntingYard::TokenRightParentheses() {
	if (expectOperand) {
		output.emplace("error", "");
	} else {
		while (!myStack.empty()) {
			if (myStack.top().first == "lpar") {
				myStack.pop();
				break;
			}

			output.emplace(myStack.top());
			myStack.pop();

			if (myStack.empty()) {
				output.emplace("error", "");
			}
		}
	}
}

void ShuntingYard::similarSign(const Token &Token) {
	output.push(myStack.top());
	myStack.pop();
	myStack.emplace(Token);
}

void ShuntingYard::higherPriority(const Token &Token) {
	while (!myStack.empty()) {
		if (myStack.top().first == "lpar") {
			break;
		}

		output.push(myStack.top());
		myStack.pop();
	}

	myStack.emplace(Token);
}

void ShuntingYard::TokenPlusOrMinus(const Token &Token) {
	if (!expectOperand) {
		if (myStack.empty() || myStack.top().first == "lpar") {
			myStack.emplace(Token);
		} else if (myStack.top().first == "opplus" || myStack.top().first == "opminus") {
			similarSign(Token);
		} else if (myStack.top().first == "opmul") {
			higherPriority(Token);
		}
		expectOperand = !expectOperand;
	} else {
		output.emplace("error", "");
	}
}

void ShuntingYard::TokenMultiply(const Token &Token) {
	if (!expectOperand) {
		if (myStack.empty() ||
		    myStack.top().first == "opplus" ||
		    myStack.top().first == "opminus" ||
		    myStack.top().first == "lpar") {
			myStack.emplace(Token);
		} else if (myStack.top().first == "opmul") {
			similarSign(Token);
		}
		expectOperand = !expectOperand;
	} else {
		output.emplace("error", "");
	}
}

void ShuntingYard::TokenEOFOrError(const Token &Token) {
	if (expectOperand) {
		output.emplace("error", "");
	} else {
		while (!myStack.empty()) {
			if (myStack.top().first == "lpar") {
				output.emplace("error", "");
			}

			output.push(myStack.top());
			myStack.pop();
		}

		output.push(Token);
	}
}

void ShuntingYard::processTokenInput(const Token &Token) {
	if (Token.first == "num" || Token.first == "id") {
		TokenOperand(Token);
	} else if (Token.first == "lpar") {
		TokenLeftParentheses(Token);
	} else if (Token.first == "rpar") {
		TokenRightParentheses();
	} else if (Token.first == "opplus" ||
			   Token.first == "opminus")
	{
		TokenPlusOrMinus(Token);
	} else if (Token.first == "opmul") {
		TokenMultiply(Token);
	} else if (Token.first == "eof" ||
			   Token.first == "error")
	{
		TokenEOFOrError(Token);
	}
}

ShuntingYard::~ShuntingYard() = default;

Token ShuntingYard::getNextToken()
{
	while (output.empty()) {
		processTokenInput(lexer.getNextToken());
	}

	if (output.front().first == "error") {
		return output.front();
	}

	auto temp = output.front();
	output.pop();
	return temp;
}
