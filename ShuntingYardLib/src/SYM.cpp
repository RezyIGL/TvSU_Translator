#include "SYM.h"
#include <iostream>

ShuntingYard::ShuntingYard(std::istream &stream) : lexer { stream } {}

void ShuntingYard::lexemOperand(const Lexem &lexem) {
	if (expectOperand) {
		output.push(lexem);
		expectOperand = !expectOperand;
	} else {
		output.emplace("error", "");
	}
}

void ShuntingYard::lexemLeftParentheses(const Lexem &lexem) {
	if (expectOperand) {
		myStack.emplace(lexem);
	} else {
		output.emplace("error", "");
	}
}

void ShuntingYard::lexemRightParentheses() {
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

void ShuntingYard::similarSign(const Lexem &lexem) {
	output.push(myStack.top());
	myStack.pop();
	myStack.emplace(lexem);
}

void ShuntingYard::higherPriority(const Lexem &lexem) {
	while (!myStack.empty()) {
		if (myStack.top().first == "lpar") {
			break;
		}

		output.push(myStack.top());
		myStack.pop();
	}

	myStack.emplace(lexem);
}

void ShuntingYard::lexemPlusOrMinus(const Lexem &lexem) {
	if (!expectOperand) {
		if (myStack.empty() || myStack.top().first == "lpar") {
			myStack.emplace(lexem);
		} else if (myStack.top().first == "opplus" || myStack.top().first == "opminus") {
			similarSign(lexem);
		} else if (myStack.top().first == "opmul") {
			higherPriority(lexem);
		}
		expectOperand = !expectOperand;
	} else {
		output.emplace("error", "");
	}
}

void ShuntingYard::lexemMultiply(const Lexem &lexem) {
	if (!expectOperand) {
		if (myStack.empty() ||
		    myStack.top().first == "opplus" ||
		    myStack.top().first == "opminus" ||
		    myStack.top().first == "lpar") {
			myStack.emplace(lexem);
		} else if (myStack.top().first == "opmul") {
			similarSign(lexem);
		}
		expectOperand = !expectOperand;
	} else {
		output.emplace("error", "");
	}
}

void ShuntingYard::lexemEOFOrError(const Lexem &lexem) {
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

		output.push(lexem);
	}
}

void ShuntingYard::processLexemInput(const Lexem &lexem) {
	if (lexem.first == "num" || lexem.first == "id") {
		lexemOperand(lexem);
	} else if (lexem.first == "lpar") {
		lexemLeftParentheses(lexem);
	} else if (lexem.first == "rpar") {
		lexemRightParentheses();
	} else if (lexem.first == "opplus" ||
			   lexem.first == "opminus")
	{
		lexemPlusOrMinus(lexem);
	} else if (lexem.first == "opmul") {
		lexemMultiply(lexem);
	} else if (lexem.first == "eof" ||
			   lexem.first == "error")
	{
		lexemEOFOrError(lexem);
	}
}

ShuntingYard::~ShuntingYard() = default;

Lexem ShuntingYard::getNextLexem()
{
	while (output.empty()) {
		processLexemInput(lexer.getNextLexem());
	}

	if (output.front().first == "error") {
		return output.front();
	}

	auto temp = output.front();
	output.pop();
	return temp;
}
