#ifndef TVSU_TRANSLATOR_LL_H
#define TVSU_TRANSLATOR_LL_H

#include "Lexer.h"
#include <queue>
#include <fstream>

class LL {
public:
	explicit LL(std::istream&, const std::string &);
	virtual ~LL();
	void validate();
private:
	void nextGraphState(const int &a);
	void eraseTrash(const std::vector<int>::iterator &da);
	void generateString(const std::string&);
	std::queue<std::string> finalOutput;
	std::string _input;
	std::ofstream myStream;
	std::vector<int> states;
	std::vector<int>::iterator graphIt;
	void rollbackIter();
	Lexer lexer;
	std::vector<Token> myQueue;
	std::vector<Token>::iterator it;
	void nextToken();
	void rollBackChanges(std::vector<Token>::iterator);
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
	bool StmtList();
	bool Stmt();
	bool DeclareStmt();
	bool Type();
	bool DeclareStmtList();
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
