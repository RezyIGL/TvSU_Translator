#ifndef TVSU_TRANSLATOR_LL_H
#define TVSU_TRANSLATOR_LL_H

#include "Lexer.h"
#include <queue>
#include <fstream>

using FT = std::pair<bool, std::string>;

class LL {
public:
	explicit LL(std::istream&, const std::string &, const std::string &);
	void validate();
private:
	// Semantic analyze
	struct Atom {
		std::string context;
		std::string text;
		std::string first;
		std::string second;
		std::string third;
	};
	std::string newLabel();
	std::string alloc(const std::string &scope);
	std::string addVar(const std::string &name, const std::string &scope, const std::string &type, const std::string &init);
	std::string addFunc(const std::string &name, const std::string &scope, const std::string &type, const std::string &length);
	std::string checkVar(const std::string &scope, const std::string &name);
	std::string checkFunc(const std::string &name, const std::string &len);
	std::vector<Atom> atoms;
	std::string _atomsInput;
	void generateAtom(const std::string &context, const std::string &text, const std::string &first, const std::string &second,const std::string &third);
	int LabelCnt = 1;
	int NewVarCnt = 1;

	struct VarOrFunc {
		std::string name;
		std::string scope;
		std::string type;
		std::string init;
		std::string kind;
		int cnt;
	};

	std::map<std::string, std::vector<VarOrFunc>> AtomicMap;
	int AtomicMapCnt = 0;

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

	FT Expr(const std::string &context);
	FT Expr7(const std::string &context);
	FT Expr7List(const std::string &context, const std::string &funcID);
	FT Expr6(const std::string &context);
	FT Expr6List(const std::string &context, const std::string &funcID);
	FT Expr5(const std::string &context);
	FT Expr5List(const std::string &context, const std::string &funcID);
	FT Expr4(const std::string &context);
	FT Expr4List(const std::string &context, const std::string &funcID);
	FT Expr3(const std::string &context);
	FT Expr3List(const std::string &context, const std::string &funcID);
	FT Expr2(const std::string &context);
	FT Expr1(const std::string &context);
	FT Expr1List(const std::string &context, const std::string &funcID);
	FT ArgList(const std::string &context);
	FT ArgListList(const std::string &context);
};


#endif //TVSU_TRANSLATOR_LL_H
