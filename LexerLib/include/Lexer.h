#ifndef LEXER_LEXER_H
#define LEXER_LEXER_H

#include "LexGraph.h"

using Lexem = std::pair<std::string, std::string>;

class Lexer {
private:
	std::istream &stream;
	char buffer;
	std::string value;
	int state = 0;
	void nextLexem(const std::vector<E> &edges, Lexem &lexem);
	void bufferNext();

public:
	Lexer(std::istream& stream);
	Lexem getNextLexem();

};

#endif //LEXER_LEXER_H
