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
        mxName = i.name.length() > mxName ? i.name.length() : mxName;
        mxCnt = std::to_string(i.cnt).length() > mxCnt ? std::to_string(i.cnt).length() : mxCnt;
        mxKind = i.kind.length() > mxKind ? i.kind.length() : mxKind;
        mxType = i.type.length() > mxType ? i.type.length() : mxType;
        mxInit = i.init.length() > mxInit ? i.init.length() : mxInit;
        mxLen = i.length.length() > mxLen ? i.length.length() : mxLen;
        mxScope = i.scope.length() > mxScope ? i.scope.length() : mxScope;
    }

    int maxLineSize = mxName + mxCnt + mxKind + mxType + mxInit + mxLen + mxScope;

    std::string headliner = "Name : Code : Class : Type : Init : Length : Scope";
    std::string separator(headliner.size() + maxLineSize, '=');

    myStream.open(_atomsInput);

    myStream << separator << std::endl << headliner << std::endl << std::endl;

    for (const auto &i : sortedAtomsVector) {

        std::string name(mxName, ' ');
        name.replace(name.begin(), name.begin() + i.name.size(), i.name);

        std::string cnt(mxCnt, ' ');
        cnt.replace(cnt.begin(), cnt.begin() + std::to_string(i.cnt).size(), std::to_string(i.cnt));

        std::string kind(mxKind, ' ');
        kind.replace(kind.begin(), kind.begin() + i.kind.size(), i.kind);

        std::string type(mxType, ' ');
        type.replace(type.begin(), type.begin() + i.type.size(), i.type);

        std::string init(mxInit, ' ');
        init.replace(init.begin(), init.begin() + i.init.size(), i.init + "'");

        std::string length(mxLen, ' ');
        length.replace(length.begin(), length.begin() + i.length.size(), i.length);

        std::string scope(mxName, ' ');
        scope.replace(scope.begin(), scope.begin() + i.scope.size(), i.scope);

        myStream << name << " : " << cnt << " : " << kind << " : " << type << " : '" << init << " : "
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