#ifndef TVSU_TRANSLATOR_LL_H
#define TVSU_TRANSLATOR_LL_H

#include "Lexer.h"
#include <queue>
#include <fstream>

class LL {
public:
	explicit LL(std::istream&, const std::string &);
	void validate();
private:
	// Graph printer
	void nextGraphState(const int &a);
	void rollbackIter();

	void generateString(const std::string&);

	std::string _input;
	std::ofstream myStream;
	std::vector<std::string> outputVector;

	std::vector<int> states;
	std::vector<int>::iterator graphIt;

	int outVecCnt = 0;

	// Syntax analyzer
	Lexer lexer;

	std::vector<Token> myQueue;
	std::vector<Token>::iterator it;

	void nextToken();
	void rollBackChanges(std::vector<Token>::iterator);

	bool StmtList();
	bool Stmt();
	bool DeclareStmt();
	bool DeclareStmtList();
	bool AssignOrCallOp();
	bool AssignOrCall();
	bool AssignOrCallList();
	bool WhileOp();
	bool ForOp();
	void ForInit();
	void ForExp();
	bool ForLoop();
	bool IfOp();
	bool ElsePart();
	bool SwitchOp();
	bool Cases();
	bool CasesList();
	bool ACase();
	bool InOp();
	bool OutOp();
	bool Type();
	bool DeclareVarList();
	bool InitVar();
	bool ParamList();
	bool ParamListList();

	bool Expr();
	bool Expr7();
	bool Expr7List();
	bool Expr6();
	bool Expr6List();
	bool Expr5();
	bool Expr5List();
	bool Expr4();
	bool Expr4List();
	bool Expr3();
	bool Expr3List();
	bool Expr2();
	bool Expr1();
	bool Expr1List();
	bool ArgList();
	bool ArgListList();
};


#endif //TVSU_TRANSLATOR_LL_H
