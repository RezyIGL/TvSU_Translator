#include "Translator.h"

// Private function to print Atoms
bool LL::_printAtoms() {

	if (!generateAtoms()) return false;

    // Max lengths
    int mxName = 0;
    int mxCnt = 0;
    int mxKind = 0;
    int mxType = 0;
    int mxInit = 0;
    int mxLen = 0;
    int mxScope = 0;

    for (const auto &i: sortedAtomsVector) {
        mxName = std::max(std::max((int) i.name.length(), mxName), 4);
        mxCnt = std::max(std::max((int) std::to_string(i.cnt).length(), mxCnt), 4);
        mxKind = std::max(std::max((int) i.kind.length(), mxKind), 5);
        mxType = std::max(std::max((int) i.type.length(), mxType), 4);
        mxInit = std::max(std::max((int) i.init.length() + 2, mxInit), 4);
        mxLen = std::max(std::max((int) i.length.length(), mxLen), 6);
        mxScope = std::max(std::max((int) i.scope.length(), mxScope), 5);
    }

    int maxLineSize = mxName + mxCnt + mxKind + mxType + mxInit + mxLen + mxScope;

    std::string headliner(maxLineSize * 2, ' ');
    std::string separator(maxLineSize + 26, '=');

    auto wordIter = headliner.begin();

    headliner.replace(wordIter, wordIter + mxName, "Name");
    headliner.replace(wordIter + mxName, wordIter + mxName + 2, " :");

    wordIter += mxName + 3;

    headliner.replace(wordIter, wordIter + mxCnt, "Code");
    headliner.replace(wordIter + mxCnt, wordIter + mxCnt + 2, " :");

    wordIter += mxCnt + 3;

    headliner.replace(wordIter, wordIter + mxKind, "Class");
    headliner.replace(wordIter + mxKind, wordIter + mxKind + 2, " :");

    wordIter += mxKind + 3;

    headliner.replace(wordIter, wordIter + mxType, "Type");
    headliner.replace(wordIter + mxType, wordIter + mxType + 2, " :");

    wordIter += mxType + 3;

    headliner.replace(wordIter, wordIter + mxInit, "Init");
    headliner.replace(wordIter + mxInit, wordIter + mxInit + 2, " :");

    wordIter += mxInit + 3;

    headliner.replace(wordIter, wordIter + mxLen, "Length");
    headliner.replace(wordIter + mxLen, wordIter + mxLen + 2, " :");

    wordIter += mxLen + 3;

    headliner.replace(wordIter, wordIter + mxScope, "Scope");

    myStream.open(_atomsInput);

    myStream << separator << std::endl << headliner << std::endl << std::endl;

    for (const auto &i : sortedAtomsVector) {
        std::string name(mxName, ' ');
        name.replace(name.begin(), name.begin() + i.name.length(), i.name);

        std::string cnt(mxCnt, ' ');
        cnt.replace(cnt.begin(), cnt.begin() + (int) std::to_string(i.cnt).length(), std::to_string(i.cnt));

        std::string kind(mxKind, ' ');
        kind.replace(kind.begin(), kind.begin() + i.kind.length(), i.kind);

        std::string type(mxType, ' ');
        type.replace(type.begin(), type.begin() + i.type.length(), i.type);

        std::string init(mxInit, ' ');
        init.replace(init.begin(), init.begin() + i.init.length(), i.init);

        std::string length(mxLen, ' ');
        length.replace(length.begin(), length.begin() + i.length.length(), i.length);

        std::string scope(mxName, ' ');
        scope.replace(scope.begin(), scope.begin() + i.scope.length(), i.scope);

        myStream << name << " : " << cnt << " : " << kind << " : " << type << " : " << init << " : "
                 << length << " : " << scope << std::endl;
    }

    myStream << std::endl << separator << std::endl << std::endl;

	myStream << "Context: Atom\n" << std::endl;

	for (const auto &i: atoms) {
		myStream << i.context << ": ("
		         << i.text << ","
		         << i.first << ","
		         << i.second << ","
		         << i.third << ")" << std::endl;
	}

    myStream << std::endl << separator;

	myStream.close();

	return true;
}