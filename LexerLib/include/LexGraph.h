#ifndef LEXER_LEXGRAPH_H
#define LEXER_LEXGRAPH_H

#include <map>
#include <vector>
#include <string>

struct E {
	std::string filter;
	bool read;
	bool save;
	int nextState;
	std::string Token;
	bool clearBuffer = false;
};

std::map<int, std::vector<E>> getGraph();

extern std::map<int, std::vector<E>> TokenFSM;

#endif //LEXER_LEXGRAPH_H
