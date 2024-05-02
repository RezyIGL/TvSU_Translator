#include "Translator.h"

// Private function to print Atoms
bool LL::_printAtoms() {

	if (!generateAtoms()) return false;

	myStream.open(_atomsInput);
	myStream << "Context: Atom\n" << std::endl;

	for (const auto &i: atoms) {
		myStream << i.context << ": ("
		         << i.text << ","
		         << i.first << ","
		         << i.second << ","
		         << i.third << ")" << std::endl;
	}

	myStream << "\n=============================================\nName : Code : Class : Type : Init : Length : Scope \n\n";

	for (const auto &i: sortedAtomsVector) {
		myStream << i.name << " : '" << i.cnt << "' : " << i.kind << " : " << i.type << " : " << i.init << " : "
                 << i.length << " : " << i.scope << std::endl;
	}

	myStream.close();

	return true;
}