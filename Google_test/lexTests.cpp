#include "Lexer.h"
#include <gtest/gtest.h>
#include <sstream>

using Lexem = std::pair<std::string, std::string>;

namespace lex_tests {

	TEST(CorrectCode, BasicProgram)
	{
		using namespace std;
		string prog = R"(
int main() {
    return 0;
}
)";
		vector<Lexem> correct = {
				{ "kwint", "" },
				{ "id", "main" },
				{ "lpar", "" },
				{ "rpar", "" },
				{ "lbrace", "" },
				{ "kwreturn", "" },
				{ "num", "0" },
				{ "semicolon", "" },
				{ "rbrace", "" },
				{ "eof", "" }
		};

		stringstream stream { prog };
		Lexer lexer(stream);

		for (auto&& lex : correct) {
			EXPECT_EQ(lex, lexer.getNextLexem());
		}
	}

	TEST(CorrectCode, ComplexProgram)
	{
		using namespace std;
		string prog = R"(
int fib(int n) {
    if (n < 2) return n;
    return fib(n - 1) + fib(n-2);
}

int is_even(int x) {
    while (x > 0) x = x - 2;
    return x == 0;
}

int main() {
    int n;
    in n;
    for (int i = 0; i < n; i++) {
        if (is_even(i)) {
            out fib(i);
        }
    }
    return 0;
}
)";
		Lexem tokens[] = {
				// int fib(int n) {
				{ "kwint", "" },
				{ "id", "fib" },
				{ "lpar", "" },
				{ "kwint", "" },
				{ "id", "n" },
				{ "rpar", "" },
				{ "lbrace", "" },

				// if (n < 2) return n;
				{ "kwif", "" },
				{ "lpar", "" },
				{ "id", "n" },
				{ "oplt", "" },
				{ "num", "2" },
				{ "rpar", "" },
				{ "kwreturn", "" },
				{ "id", "n" },
				{ "semicolon", "" },

				// return fib(n - 1)
				{ "kwreturn", "" },
				{ "id", "fib" },
				{ "lpar", "" },
				{ "id", "n" },
				{ "opminus", "" },
				{ "num", "1" },
				{ "rpar", "" },

				//  + fib(n-2);
				{ "opplus", "" },
				{ "id", "fib" },
				{ "lpar", "" },
				{ "id", "n" },
				{ "num", "-2" },
				{ "rpar", "" },
				{ "semicolon", "" },

				// }
				{ "rbrace", "" },

				// int is_even(int x) {
				{ "kwint", "" },
				{ "id", "is_even" },
				{ "lpar", "" },
				{ "kwint", "" },
				{ "id", "x" },
				{ "rpar", "" },
				{ "lbrace", "" },

				// while (x > 0)
				{ "kwwhile", "" },
				{ "lpar", "" },
				{ "id", "x" },
				{ "opgt", "" },
				{ "num", "0" },
				{ "rpar", "" },

				// x = x - 2;
				{ "id", "x" },
				{ "opassign", "" },
				{ "id", "x" },
				{ "opminus", "" },
				{ "num", "2" },
				{ "semicolon", "" },

				//    return x == 0;
				{ "kwreturn", "" },
				{ "id", "x" },
				{ "opeq", "" },
				{ "num", "0" },
				{ "semicolon", "" },

				//}
				{ "rbrace", "" },
				// int main() {
				{ "kwint", "" },
				{ "id", "main" },
				{ "lpar", "" },
				{ "rpar", "" },
				{ "lbrace", "" },

				//    int n;
				{ "kwint", "" },
				{ "id", "n" },
				{ "semicolon", "" },

				//    in n;
				{ "kwin", "" },
				{ "id", "n" },
				{ "semicolon", "" },

				//    for (int i = 0;
				{ "kwfor", "" },
				{ "lpar", "" },
				{ "kwint", "" },
				{ "id", "i" },
				{ "opassign", "" },
				{ "num", "0" },
				{ "semicolon", "" },

				//  i < n; i++) {
				{ "id", "i" },
				{ "oplt", "" },
				{ "id", "n" },
				{ "semicolon", "" },

				{ "id", "i" },
				{ "opinc", "" },
				{ "rpar", "" },
				{ "lbrace", "" },

				//        if (is_even(i)) {
				{ "kwif", "" },
				{ "lpar", "" },
				{ "id", "is_even" },
				{ "lpar", "" },
				{ "id", "i" },
				{ "rpar", "" },
				{ "rpar", "" },
				{ "lbrace", "" },

				//            out fib(i);
				{ "kwout", "" },
				{ "id", "fib" },
				{ "lpar", "" },
				{ "id", "i" },
				{ "rpar", "" },
				{ "semicolon", "" },

				//        }
				{ "rbrace", "" },
				//    }
				{ "rbrace", "" },

				//    return 0;
				{ "kwreturn", "" },
				{ "num", "0" },
				{ "semicolon", "" },

				//}
				{ "rbrace", "" },
				{ "eof", "" }
		};

		vector<Lexem> correct(begin(tokens), end(tokens));

		stringstream stream { prog };
		Lexer lexer(stream);

		for (int i = 0; i < correct.size(); ++i) {
			EXPECT_EQ(correct[i], lexer.getNextLexem()) << "token #" << i;
		}
	}

	TEST(TrickyCode, BasicProgram)
	{
		using namespace std;
		string prog = "int main(){return 0;}";
		vector<Lexem> correct = {
				{ "kwint", "" },
				{ "id", "main" },
				{ "lpar", "" },
				{ "rpar", "" },
				{ "lbrace", "" },
				{ "kwreturn", "" },
				{ "num", "0" },
				{ "semicolon", "" },
				{ "rbrace", "" },
				{ "eof", "" }
		};

		stringstream stream { prog };
		Lexer lexer(stream);

		for (auto&& lex : correct) {
			EXPECT_EQ(lex, lexer.getNextLexem());
		}
	}

	TEST(TrickyCode, MultilineString)
	{
		using namespace std;
		string prog = R"(out "Hello
world
'
for everyone!!!
")";
		string content = "Hello\n"
		                 "world\n"
		                 "'\n"
		                 "for everyone!!!\n";
		vector<Lexem> correct = {
				{ "kwout", "" },
				{ "str", content },
				{ "eof", "" }
		};

		stringstream stream { prog };
		Lexer lexer(stream);

		for (auto&& lex : correct) {
			EXPECT_EQ(lex, lexer.getNextLexem());
		}
	}

	TEST(TrickyCode, Expressions)
	{
		using namespace std;
		string prog = "int a=0;int aa=182;out aa+a==-123;";
		vector<Lexem> correct = {
				// int a=0;
				{ "kwint", "" },
				{ "id", "a" },
				{ "opassign", "" },
				{ "num", "0" },
				{ "semicolon", "" },

				// int aa=182;
				{ "kwint", "" },
				{ "id", "aa" },
				{ "opassign", "" },
				{ "num", "182" },
				{ "semicolon", "" },

				// out aa+a==-123;
				{ "kwout", "" },
				{ "id", "aa" },
				{ "opplus", "" },
				{ "id", "a" },
				{ "opeq", "" },
				{ "num", "-123" },
				{ "semicolon", "" },
				{ "eof", "" }
		};

		stringstream stream { prog };
		Lexer lexer(stream);

		for (auto&& lex : correct) {
			EXPECT_EQ(lex, lexer.getNextLexem());
		}
	}

	TEST(ErrorCode, BadString)
	{
		using namespace std;
		string prog = R"(out "incomplete string;)";
		vector<Lexem> correct = {
				{ "kwout", "" },
				{ "error", "" },
		};

		stringstream stream { prog };
		Lexer lexer(stream);

		for (auto&& lex : correct) {
			EXPECT_EQ(lex, lexer.getNextLexem());
		}
	}

	TEST(ErrorCode, EmptyChar)
	{
		using namespace std;
		string prog = R"(out '')";
		vector<Lexem> correct = {
				{ "kwout", "" },
				{ "error", "" },
		};

		stringstream stream { prog };
		Lexer lexer(stream);

		for (auto&& lex : correct) {
			EXPECT_EQ(lex, lexer.getNextLexem());
		}
	}

	TEST(ErrorCode, DoubleChar)
	{
		using namespace std;
		string prog = R"(out 'xy')";
		vector<Lexem> correct = {
				{ "kwout", "" },
				{ "error", "" },
		};

		stringstream stream { prog };
		Lexer lexer(stream);

		for (auto&& lex : correct) {
			EXPECT_EQ(lex, lexer.getNextLexem());
		}
	}

	TEST(ErrorCode, SingleOperator)
	{
		using namespace std;
		string prog = R"(a | b)";
		vector<Lexem> correct = {
				{ "id", "a" },
				{ "error", "" },
		};

		stringstream stream { prog };
		Lexer lexer(stream);

		for (auto&& lex : correct) {
			EXPECT_EQ(lex, lexer.getNextLexem());
		}
	}

}