#ifndef LEXER_LEXER_H
#define LEXER_LEXER_H

#include "LexGraph.h"

using Token = std::pair<std::string, std::string>;

class Lexer {
private:
	std::istream &stream;
	char buffer;
	std::string value;
	int state = 0;
	void nextToken(const std::vector<E> &edges, Token &Token);
	void bufferNext();

public:
	explicit Lexer(std::istream& stream);
	Token getNextToken();

};

#endif //LEXER_LEXER_H
