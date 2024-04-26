#ifndef TVSU_TRANSLATOR_TRANSLATOR_H
#define TVSU_TRANSLATOR_TRANSLATOR_H

#include "Lexer.h"
#include <iostream>
#include <fstream>

using FT = std::pair<bool, std::string>;

struct Atom {
	std::string context;
	std::string text;
	std::string first;
	std::string second;
	std::string third;
};

struct VarOrFunc {
	std::string name;
	std::string scope;
	std::string type;
	std::string init;
	std::string kind;
	int cnt;
};

class LL {
public:
	// Constructor.
	explicit LL(std::istream&, const std::string&, const std::string&, const std::string&);

	// Function to say if your MiniC code contains Error
	bool checkIfCodeSyntaxIsCorrect();

	// Function to print graph of your code
	void printOutputGraph();

	// Function to print Atoms of your code
	void printAtoms();

	// Function to generate ASM i8080 code
	void printASMCode();

private:
	// Checker Check
	bool CodeIsChecked = false;
	bool CodeIsCorrect = false;
	bool AtomsGenerated = false;

	// clears files
	void clearStream(const std::string &);

	// Private function to print Atoms
	bool _printAtoms();
	bool generateAtoms();

	// Private function to print ASM i8080 code
	bool _printASMCode();

	// Private function to print Output Graph
	void _printGraph();

	// ASM i8080 helpful functions
	void loadOp(const std::string &);
	void saveOp(const std::string &);

	// variables for (MUL)
	std::string MUL_LABEL_START;
	std::string MUL_LABEL_END;
	std::string MUL_LABEL_ZERO;

	// ASM i8080 translating functions
	void MOV(const Atom &);
	void LBL(const Atom &);
	void JMP(const Atom &);
	void ADD(const Atom &);
	void SUB(const Atom &);
	void OR(const Atom &);
	void AND(const Atom &);
	void EQ(const Atom &);
	void NE(const Atom &);
	void GT(const Atom &);
	void LT(const Atom &);
	void GE(const Atom &);
	void LE(const Atom &);
	void NOT(const Atom &);
	void MUL(const Atom &);
	void IN(const Atom &);
	void OUT(const Atom &);

	// Not implemented, because we don't have good i8080 emulator
	// You can do it yourself if you want to though
	void CALL(const Atom &);
	void PARAM(const Atom &);
	void RET(const Atom &);

	// Entry point holder. If not found = "NoEntry"
	std::string entryPoint = "NoEntry";
	bool isUsed = false;
	std::string currentContext = "-1";
	bool doWeHaveLBL = false;

	// Needful variables
	std::string _atomsInput; // path to Atoms output file
	std::string _asmOut; // path to ASM i8080 output file
	std::string _graph; // path to output graph output file
	std::ofstream myStream; // path to the input file

	std::vector<Atom> atoms; // Contains all the atoms we generated
	std::vector<VarOrFunc> sortedAtomsVector; // contains sorted list of variables that we will need to allocate

	int LabelCnt = 1; // Variable to know which labels we already used
	int NewVarCnt = 1; // Variable to create new tempVariables

	std::map<std::string, std::vector<VarOrFunc>> AtomicMap; // Map: Context -> all the (functions or variables)
	int AtomicMapCnt = 0; // Amount of... idk... I forgot :(

	std::vector<std::string> outputVector; // Vector that stores all the strings of output graph
	std::vector<int> states; // states to understand which string to generate for graph
	std::vector<int>::iterator graphIterator = states.begin(); // Iterator of my graph vector

	std::vector<Token> lexemVector; // All the Tokens from Lexer we store here
	std::vector<Token>::iterator it = lexemVector.begin(); // Iterator for that vector

	int outVecCnt = 0; // The shift, if I remember correctly

	// Translation to Atoms Language
	std::string newLabel();

	std::string alloc(const std::string &scope);

	std::string
	addVar(const std::string &name, const std::string &scope, const std::string &type, const std::string &init = "0");

	std::string addFunc(const std::string &name, const std::string &type, const std::string &length = "0");

	std::string checkVar(const std::string &scope, const std::string &name);

	std::string checkFunc(const std::string &name, const std::string &len);

	void generateAtom(const std::string &context, const std::string &text, const std::string &first,
	                  const std::string &second, const std::string &third);

	// Creation of output graph
	void nextGraphState(const int &a);
	void rollbackGraphNode();
	void generateString(const std::string &);

	// Correct or not checker (everything under that)
	Lexer lexer;
	void nextToken();

	// MiniC grammar checker
	bool StmtList(const std::string &context);
	bool Stmt(const std::string &context);
	bool DeclareStmt(const std::string &context);
	bool DeclareStmtList(const std::string &context, const std::string &q, const std::string &r);
	bool AssignOrCallOp(const std::string &context);
	bool AssignOrCall(const std::string &context);
	bool AssignOrCallList(const std::string &context, const std::string &name);
	bool WhileOp(const std::string &context);
	bool ForOp(const std::string &context);
	bool ForInit(const std::string &context);
	FT ForExp(const std::string &context);
	bool ForLoop(const std::string &context);
	bool IfOp(const std::string &context);
	bool ElsePart(const std::string &context);
	bool SwitchOp(const std::string &context);
	bool Cases(const std::string &context, const std::string &p, const std::string &end);
	bool CasesList(const std::string &context, const std::string &p, const std::string &end, const std::string &def);
	FT ACase(const std::string &context, const std::string &p, const std::string &end);
	bool InOp(const std::string &context);
	bool OutOp(const std::string &context);
	bool OutOpList(const std::string &context);
	FT Type(const std::string &context);
	bool DeclareVarList(const std::string &context, const std::string &type);
	bool InitVar(const std::string &context, const std::string &r, const std::string &s);
	FT ParamList(const std::string &context);
	FT ParamListList(const std::string &context);

	// Expression checker
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

#endif //TVSU_TRANSLATOR_TRANSLATOR_H
