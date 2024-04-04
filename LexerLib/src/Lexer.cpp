#include "Lexer.h"
#include <set>
#include <iostream>

std::set<std::string> keywords = {
		"int",
		"char",
		"if",
		"else",
		"while",
		"for",
		"out",
		"in",
		"switch",
		"case",
		"return",
		"default"
};

Token Lexer::getNextToken() {
	Token Token;

	while (Token.first.empty()) {
		nextToken(getGraph()[state], Token);
	}

	return Token;
}

void Lexer::bufferNext() {
	buffer = stream.get();
}

Lexer::Lexer(std::istream &stream) : stream{stream} {
	buffer = stream.get();
}

void Lexer::nextToken(const std::vector<E> &edges, Token &Token) {
	E ruleset = {"", false, false, 0, ""};
	for (auto &e: edges) {
		if (e.filter == "LETTER") {
			if (isalpha(buffer) || buffer == '_') {
				ruleset = e;
				break;
			}
		} else if (e.filter == "EOF") {
			if (buffer == -1) {
				ruleset = e;
				break;
			}
		} else if (e.filter == "DIGIT") {
			if (isdigit(buffer)) {
				ruleset = e;
				break;
			}
		} else if (e.filter[0] == buffer) {
			ruleset = e;
			break;
		} else if (ruleset.filter.empty() && e.filter.empty()) {
			ruleset = e;
		}
	}

	if (ruleset.save) {
		value.push_back(buffer);
	}
	if (ruleset.read) bufferNext();
	if (ruleset.clearBuffer) value.clear();

	if (!ruleset.Token.empty()) {
		if (state == 21 && keywords.contains(value)) {
			Token.first = "kw" + value;
			Token.second = "";
		} else if (state == 21) {
			Token.first = "id";
			Token.second = value;
		} else {
			Token.first = ruleset.Token;
			Token.second = value;
		}
		value.clear();
	}

	state = ruleset.nextState;
}
