#include "LexGraph.h"

std::map<int, std::vector<E>> LexemFSM = {
		{0, {
				    {"(", false, false, 1, ""},
				    {")", false, false, 1, ""},
				    {"{", false, false, 1, ""},
				    {"}", false, false, 1, ""},
				    {"[", false, false, 1, ""},
				    {"]", false, false, 1, ""},
				    {";", false, false, 1, ""},
				    {",", false, false, 1, ""},
				    {".", false, false, 1, ""},
				    {">", true, false, 24, ""},
				    {"*", false, false, 1, ""},
				    {"<", true, false, 2, ""},
				    {"!", true, false, 4, ""},
				    {"=", true, false, 6, ""},
				    {"+", true, false, 8, ""},
				    {"|", true, false, 10, ""},
				    {"&", true, false, 12, ""},
				    {"'", true, false, 14, ""},
				    {"\"", true, false, 18, ""},
				    {"LETTER", false, false, 21, ""},
				    {"-", true, true, 22, ""},
				    {"DIGIT", false, false, 23, ""},
				    {" ", true, false, 0, ""},
				    {"\n", true, false, 0, ""},
				    {"\t", true, false, 0, ""},
				    {"", false, false, -1, "eof"}
		    }},
		{1, {
				    {"(", true, false, 0, "lpar"},
				    {")", true, false, 0, "rpar"},
				    {"{", true, false, 0, "lbrace"},
				    {"}", true, false, 0, "rbrace"},
				    {"[", true, false, 0, "lbracket"},
				    {"]", true, false, 0, "rbracket"},
				    {";", true, false, 0, "semicolon"},
				    {",", true, false, 0, "comma"},
				    {".", true, false, 0, "period"},
				    {"*", true, false, 0, "opmul"},
		    }},
		{2, {
				    {"=", false, false, 3, ""},
				    {"", false, false, 0, "oplt"}
		    }},
		{3, {
				    {"", true, false, 0, "ople"}
		    }},
		{4, {
				    {"=", false, false, 5, ""},
				    {"", false, false, 0, "opnot"}
		    }},
		{5, {
				    {"", true, false, 0, "opne"}
		    }},
		{6, {
				    {"=", false, false, 7, ""},
				    {"", false, false, 0, "opassign"}
		    }},
		{7, {
				    {"", true, false, 0, "opeq"}
		    }},
		{8, {
				    {"+", false, false, 9, ""},
				    {"", false, false, 0, "opplus"}
		    }},
		{9, {
				    {"", true, false, 0, "opinc"}
		    }},
		{10, {
				    {"|", false, false, 11, ""},
				    {"", false, false, -1, "error", true}
		    }},
		{11, {
				    {"", true, false, 0, "opor"}
		    }},
		{12, {
				    {"&", false, false, 13, ""},
				    {"", false, false, -1, "error", true}
		    }},
		{13, {
				    {"", true, false, 0, "opand"}
		    }},
		{14, {
				    {"'", false, false, 15, ""},
				    {"", true, true, 16, ""}
		    }},
		{15, {
				    {"", false, false, -1, "error", true}
		    }},
		{16, {
				    {"'", true, false, 17, ""},
				    {"", false, false, -1, "error", true}
		    }},
		{17, {
				    {"", false, false, 0, "char"}
		    }},
		{18, {
				    {"\"", false, false, 20, ""},
				    {"", true, true, 18, ""},
				    {"EOF", false, false, 19, ""}
		    }},
		{19, {
				    {"", false, false, -1, "error", true}
		    }},
		{20, {
				    {"", true, false, 0, "str"}
		    }},
		{21, {
				    {"DIGIT", true, true, 21, ""},
				    {"LETTER", true, true, 21, ""},
				    {"", false, false, 0, "aboba"}
		    }},
		{22, {
				    {"DIGIT", true, true, 23, ""},
				    {"", false, false, 0, "opminus", true}
		    }},
		{23, {
				    {"DIGIT", true, true, 23, ""},
				    {"", false, false, 0, "num"}
		    }},
		{24, {
				     {"=", true, false, 0, "opge"},
				     {"", false, false, 0, "opgt"}
		}}
};

std::map<int, std::vector<E>> getGraph() {
	// deepcopy
	return LexemFSM;
}