#ifndef TVSU_TRANSLATOR_LL_H
#define TVSU_TRANSLATOR_LL_H

#include "Lexer.h"
#include <queue>
#include <fstream>

using FT = std::pair<bool, std::string>;

class LL {
public:
	explicit LL(std::istream&, const std::string &, const std::string &);
	virtual ~LL() = default;
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
	std::string addFunc(const std::string &name, const std::string &type, const std::string &length);
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

	bool StmtList(const std::string &context);
	bool Stmt(const std::string &context);
	bool DeclareStmt(const std::string &context);
	bool DeclareStmtList(const std::string &context, const std::string &q, const std::string &r);
	bool AssignOrCallOp(const std::string &context);
	bool AssignOrCall(const std::string &context);
	bool AssignOrCallList(const std::string &context, const std::string &name);
	bool WhileOp(const std::string &context);
	bool ForOp(const std::string &context);
	void ForInit(const std::string &context);
	std::string ForExp(const std::string &context);
	bool ForLoop(const std::string &context);
	bool IfOp(const std::string &context);
	bool ElsePart(const std::string &context);
	bool SwitchOp(const std::string &context);
	bool Cases(const std::string &context);
	bool CasesList(const std::string &context);
	bool ACase(const std::string &context);
	bool InOp(const std::string &context);
	bool OutOp(const std::string &context);
	bool OutOpList(const std::string &context);
	FT Type(const std::string &context);
	bool DeclareVarList(const std::string &context, const std::string &type);
	bool InitVar(const std::string &context, const std::string &r, const std::string &s);
	FT ParamList(const std::string &context);
	FT ParamListList(const std::string &context);

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
