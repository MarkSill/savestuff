#include <iostream>
#include "variable.hpp"

using namespace savestuff;

int main(int argc, char *argv[]) {
	Variable global(TABLE);
	global.loadFromFile("testin.svstf");
	global.saveToFile("testout.svstf");
	global.saveToFile("testout.mini.svstf", true);
	std::cout << "Global environment: " << global << "\n";
	std::cout << "Nested table: " << *global["a_table"] << "\n";
	std::cout << "First element of nested table: " << *global["a_table"]->at(0.) << "\n";
	std::cout << "Number of elements in global space (not counting nested): " << global.length() << "\n";
	return 0;
}
