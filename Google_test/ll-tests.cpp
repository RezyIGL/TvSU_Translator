#include "ll.h"
#include <gtest/gtest.h>

namespace LL_tests {

	TEST(LL_CorrectExpression, FivePlusFive)
	{
		using namespace std;
		string expression = "5 + 5";
		string output = "Accepted!";

		stringstream stream { expression };
		LL ll(stream);

		EXPECT_EQ(ll.validate(), output);
	}

	TEST(LL_CorrectExpression, SomethingBig)
	{
		using namespace std;
		string expression = "5 + 5 - 5 * 5 || (a + (b * c) && 2)";
		string output = "Accepted!";

		stringstream stream { expression };
		LL ll(stream);

		EXPECT_EQ(ll.validate(), output);
	}

	TEST(LL_CorrectExpression, MultOfSums)
	{
		using namespace std;
		string expression = "(1 + 2)*(3 + 4)";
		string output = "Accepted!";

		stringstream stream { expression };
		LL ll(stream);

		EXPECT_EQ(ll.validate(), output);
	}

	TEST(LL_CorrectExpression, SumAndMult)
	{
		using namespace std;
		string expression = "1+ 2*2";
		string output = "Accepted!";

		stringstream stream { expression };
		LL ll(stream);

		EXPECT_EQ(ll.validate(), output);
	}

	TEST(LL_CorrectExpression, AllMults)
	{
		using namespace std;
		string expression = "1*2*3*4";
		string output = "Accepted!";

		stringstream stream { expression };
		LL ll(stream);

		EXPECT_EQ(ll.validate(), output);
	}

	TEST(LL_IncorrectExpression, NotCompleted)
	{
		using namespace std;
		string expression = "5 + 5 -";
		string output = "Incorrect Expression!";

		stringstream stream { expression };
		LL ll(stream);

		EXPECT_EQ(ll.validate(), output);
	}

	TEST(LL_IncorrectExpression, WrongParatheses)
	{
		using namespace std;
		string expression = "(a + 2";
		string output = "Incorrect Expression!";

		stringstream stream { expression };
		LL ll(stream);

		EXPECT_EQ(ll.validate(), output);
	}

	TEST(LL_IncorrectExpression, BigWrong1)
	{
		using namespace std;
		string expression = "1 || 2 && 3 == 4 != 5 < 6 + 7 - 8 * 9 !10";
		string output = "Incorrect Expression!";

		stringstream stream { expression };
		LL ll(stream);

		EXPECT_EQ(ll.validate(), output);
	}

	TEST(LL_IncorrectExpression, BigWrong2)
	{
		using namespace std;
		string expression = "1 || 2 && 3 == 4 != 5 < 6 + 7 - 8 * !9++";
		string output = "Incorrect Expression!";

		stringstream stream { expression };
		LL ll(stream);

		EXPECT_EQ(ll.validate(), output);
	}

	TEST(LL_IncorrectExpression, BigWrong3)
	{
		using namespace std;
		string expression = "1 || 2 && 3 == 4 != 5 < 6 + 7 - 8 * !9++ + (a)";
		string output = "Incorrect Expression!";

		stringstream stream { expression };
		LL ll(stream);

		EXPECT_EQ(ll.validate(), output);
	}
}