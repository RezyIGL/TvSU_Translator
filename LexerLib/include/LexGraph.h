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
	std::string lexem;
	bool clearBuffer = false;
};

std::map<int, std::vector<E>> getGraph();

extern std::map<int, std::vector<E>> LexemFSM;

#endif //LEXER_LEXGRAPH_H
