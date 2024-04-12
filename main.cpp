#include "ll.h"
#include <fstream>

int main() {
    std::string output = R"(E:\CLionProjects\TvSU_Translator\output.txt)";
    std::fstream input(R"(E:\CLionProjects\TvSU_Translator\input.minic)");

	/*
	 * Currently you can write:
	 *
	 * int id; | char id; | int id(ParamList) { StmtList } | char id(ParamList) { StmtList }
	 * while (Expr) Stmt
	 * if (Expr) Stmt else Stmt | if (Expr) Stmt
	 * in id;
	 * out Expr;
	 * ;
	 * { StmtList };
	 * return Expr;
	 * AssignOrCallOp
	 *
	 * TODO: Make output for -> ForOp && SwitchOp!!
	 */

	LL ll(input, output);
	ll.validate();

	return 0;
}
