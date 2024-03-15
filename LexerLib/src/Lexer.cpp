#include "Lexer.h"
#include <set>
#include <iostream>

using Lexem = std::pair<std::string, std::string>;

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
		"return"
};

Lexem Lexer::getNextLexem() {
	Lexem lexem;

	while (lexem.first == "") {
		nextLexem(getGraph()[state], lexem);
	}

	return lexem;
}

void Lexer::bufferNext() {
	buffer = stream.get();
}

Lexer::Lexer(std::istream &stream) : stream{stream} {
	buffer = stream.get();
}

void Lexer::nextLexem(const std::vector<E> &edges, Lexem &lexem) {
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

	if (!ruleset.lexem.empty()) {
		if (state == 21 && keywords.contains(value)) {
			lexem.first = "kw" + value;
			lexem.second = "";
		} else if (state == 21) {
			lexem.first = "id";
			lexem.second = value;
		} else {
			lexem.first = ruleset.lexem;
			lexem.second = value;
		}
		value.clear();
	}

	state = ruleset.nextState;
}
