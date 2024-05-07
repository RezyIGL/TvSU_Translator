#include "Translator.h"
#include "iomanip"

struct mxCnts {
    ulong mxName = 4;
    ulong mxCnt = 2;
    ulong mxKind = 5;
    ulong mxType = 4;
    ulong mxInit = 4;
    ulong mxLen = 6;
    ulong mxScope = 5;
    ulong MaxSize = 0;
};

struct mxAtoms {
    ulong mxScope = 7;
    ulong mxAtomSize = 5;
};

std::string generateBorderForTable(const mxCnts &mx, const std::string &border = "bottom") {

    std::string toReturn;
    std::string LB = border == "top" ? "╔" : border == "mid" ? "╠" : "╚";
    std::string RB = border == "top" ? "╗" : border == "mid" ? "╣" : "╝";
    std::string SEP = border == "top" ? "╦" : border == "mid" ? "╬" : "╩";

    toReturn += LB;
    for (int i = 0; i < mx.mxName + 2; i++) toReturn += "═";
    toReturn += SEP;
    for (int i = 0; i < mx.mxCnt + 2; i++) toReturn += "═";
    toReturn += SEP;
    for (int i = 0; i < mx.mxKind + 2; i++) toReturn += "═";
    toReturn += SEP;
    for (int i = 0; i < mx.mxType + 2; i++) toReturn += "═";
    toReturn += SEP;
    for (int i = 0; i < mx.mxInit + 2; i++) toReturn += "═";
    toReturn += SEP;
    for (int i = 0; i < mx.mxLen + 2; i++) toReturn += "═";
    toReturn += SEP;
    for (int i = 0; i < mx.mxScope + 2; i++) toReturn += "═";
    toReturn += RB;

    return toReturn;
}

std::string generateBorderForAtoms(const mxAtoms &mx, const std::string &border = "bottom") {

    std::string toReturn;
    std::string LB = border == "top" ? "╔" : border == "mid" ? "╠" : "╚";
    std::string RB = border == "top" ? "╗" : border == "mid" ? "╣" : "╝";
    std::string SEP = border == "top" ? "╦" : border == "mid" ? "╬" : "╩";

    toReturn += LB;
    for (int i = 0; i < mx.mxScope + 2; i++) toReturn += "═";
    toReturn += SEP;
    for (int i = 0; i < mx.mxAtomSize + 2; i++) toReturn += "═";
    toReturn += RB;

    return toReturn;
}

// Private function to print Atoms
bool LL::_printAtoms() {

	if (!generateAtoms()) return false;

    mxCnts mx;

    for (const auto &i: sortedAtomsVector) {
        mx.mxName = i.name.length() > mx.mxName ? i.name.length() : mx.mxName;
        mx.mxCnt = std::to_string(i.cnt).length() > mx.mxCnt ? std::to_string(i.cnt).length() : mx.mxCnt;
        mx.mxKind = i.kind.length() > mx.mxKind ? i.kind.length() : mx.mxKind;
        mx.mxInit = i.init.length() > mx.mxInit ? i.init.length() : mx.mxInit;
        mx.mxLen = i.length.length() > mx.mxLen ? i.length.length() : mx.mxLen;
        mx.mxScope = i.scope.length() > mx.mxScope ? i.scope.length() : mx.mxScope;
    }

    mx.MaxSize = mx.mxName + mx.mxCnt + mx.mxKind + mx.mxType + mx.mxInit + mx.mxLen + mx.mxScope + 14 + 8;

    myStream.open(_atomsInput);

    // Top of table
    myStream << generateBorderForTable(mx, "top") << std::endl;

    // Text part
    myStream << std::left << std::setw(mx.mxName + 5) << "║ Name";
    myStream << std::left << std::setw(mx.mxCnt + 5) << "║ ID";
    myStream << std::left << std::setw(mx.mxKind + 5) << "║ Class";
    myStream << std::left << std::setw(mx.mxType + 5) << "║ Type";
    myStream << "║" << std::right << std::setw(mx.mxInit + 2) << "Init ";
    myStream << "║" << std::right << std::setw(mx.mxLen + 2) << "Length ";
    myStream << "║" << std::right << std::setw(mx.mxScope + 1) << "Scope" << " ║" << std::endl;

    // separator from name and content
    myStream << generateBorderForTable(mx, "mid") << std::endl;

    // Cheking if not last line
    int _cnt = 0;

    // All the items of table print
    for (const auto &tableParticipant : sortedAtomsVector) {
        myStream << "║ " << std::left << std::setw(mx.mxName + 1) << tableParticipant.name;
        myStream << "║ " << std::left << std::setw(mx.mxCnt + 1) << tableParticipant.cnt;
        myStream << "║ " << std::left << std::setw(mx.mxKind + 1) << tableParticipant.kind;
        myStream << "║ " << std::left << std::setw(mx.mxType + 1) << tableParticipant.type.substr(2, tableParticipant.type.size());
        myStream << "║" << std::right << std::setw(mx.mxInit + 1) << tableParticipant.init << " ";
        myStream << "║" << std::right << std::setw(mx.mxLen + 1) << tableParticipant.length << " ";
        myStream << "║" << std::right << std::setw(mx.mxScope + 1) << tableParticipant.scope << " ║" << std::endl;
        if (_cnt != sortedAtomsVector.size() - 1) myStream << generateBorderForTable(mx, "mid") << std::endl;
        _cnt++;
    }

    // Bottom of table
    myStream << generateBorderForTable(mx) << std::endl << std::endl;

    // Atoms printing
    mxAtoms mx2;

    for (const auto &atom : atoms) {
        ulong rh = atom.context.length()
                     + atom.text.length()
                     + atom.first.length()
                     + atom.second.length()
                     + atom.third.length();
        mx2.mxAtomSize = std::max(mx2.mxAtomSize, rh);
    }

    mx2.mxAtomSize = mx2.mxAtomSize + 4;

    myStream << generateBorderForAtoms(mx2, "top") << std::endl;
    myStream << std::left << std::setw(mx2.mxScope + 5) << "║ Context";
    myStream << std::left << std::setw(mx2.mxAtomSize + 5) << "║ Atom" << "║" << std::endl;
    myStream << generateBorderForAtoms(mx2, "mid") << std::endl;

    // All the atoms print
    _cnt = 0;
    for (const auto &atom : atoms) {
        myStream << "║ " << std::left << std::setw(mx2.mxScope + 1) << atom.context;
        myStream << "║ " << std::left << std::setw(mx2.mxAtomSize + 1) << "("
                 + atom.text + ","
                 + atom.first + ","
                 + atom.second + ","
                 + atom.third + ")"
                 << "║" << std::endl;
        if (_cnt != atoms.size() - 1) myStream << generateBorderForAtoms(mx2, "mid") << std::endl;
        _cnt++;
    }

    myStream << generateBorderForAtoms(mx2, "bottom");

	myStream.close();

	return true;
}