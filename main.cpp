#include <iostream>
#include <map>
#include <sstream>
#include "SYM.h"

std::map<std::vector<Token>, Token> rules = {
		{{{"E7",    ""}},                                {"E",  ""}},
		{{{"E7",    ""}, {"opor",    ""}, {"E6",   ""}}, {"E7", ""}},
		{{{"E6",    ""}},                                {"E7", ""}},
		{{{"E6",    ""}, {"opand",   ""}, {"E5",   ""}}, {"E6", ""}},
		{{{"E5",    ""}},                                {"E6", ""}},
		{{{"E4",    ""}, {"opeq",    ""}, {"E4",   ""}}, {"E5", ""}},
		{{{"E4",    ""}, {"opne",    ""}, {"E4",   ""}}, {"E5", ""}},
		{{{"E4",    ""}, {"opgt",    ""}, {"E4",   ""}}, {"E5", ""}},
		{{{"E4",    ""}, {"oplt",    ""}, {"E4",   ""}}, {"E5", ""}},
		{{{"E4",    ""}, {"ople",    ""}, {"E4",   ""}}, {"E5", ""}},
		{{{"E4",    ""}},                                {"E5", ""}},
		{{{"E4",    ""}, {"opplus",  ""}, {"E3",   ""}}, {"E4", ""}},
		{{{"E4",    ""}, {"opminus", ""}, {"E3",   ""}}, {"E4", ""}},
		{{{"E3",    ""}},                                {"E4", ""}},
		{{{"E3",    ""}, {"opmul",   ""}, {"E2",   ""}}, {"E3", ""}},
		{{{"E2",    ""}},                                {"E3", ""}},
		{{{"opnot", ""}, {"E1",      ""}},               {"E2", ""}},
		{{{"E1",    ""}},                                {"E2", ""}},
		{{{"opinc", ""}, {"id",      ""}},               {"E1", ""}},
		{{{"id",    ""}, {"opinc",   ""}},               {"E1", ""}},
		{{{"lpar",  ""}, {"E",       ""}, {"rpar", ""}}, {"E1", ""}},
		{{{"num",   ""}},                                {"E1", ""}},
		{{{"id",    ""}},                                {"E1", ""}},
};

bool vectorContains(const std::vector<Token>& vec, const std::vector<Token>& target) {
	if (vec.size() != target.size())
		return false;

	for (size_t i = 0; i < vec.size(); ++i) {
		if (vec[i].first != target[i].first) {
			return false;
		}
	}
	return true;
}

std::vector<Token> mapContains(const std::map<std::vector<Token>, Token>& myMap, const std::vector<Token>& target) {
	for (const auto& pair : myMap) {
		if (vectorContains(pair.first, target)) {
			return pair.first;
		}
	}
	return {};
}

void foo(std::vector<Token> &LRTokens) {
	auto it = LRTokens.begin();

	while (true) {
		if (it == LRTokens.end()) {
			return;
		}

		auto foundKey = mapContains(rules, std::vector<Token>(it, LRTokens.end()));

		if (!foundKey.empty()) {
			LRTokens.erase(it, LRTokens.end());
			LRTokens.insert(it, rules[foundKey]);
			return;
		}

		it++;
	}
}

int main() {
	// Try this input: a ++ - Wouldn't work :(
	// Anything else will work only with precise to E1 :p
	std::string input = "1 + a";
	std::stringstream ss(input);

	// temp container for tokens from input
	std::vector<Token> LRTokens;

	// Token container
	Token token;

	// Lexer class to get tokens
	Lexer lexer(ss);

	// Read input until not error or eof
	while (token.first != "eof" && token.first != "error") {
		token = lexer.getNextToken();

		if (token.first != "eof") LRTokens.push_back(token);
		else break;

		foo(LRTokens);
	}

	// print the result
	for (const auto &i : LRTokens) {
		std::cout << "{\"" << i.first << "\", \"" << i.second << "\"}," << std::endl;
	}
}
