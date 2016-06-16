#include "properties.hpp"

using namespace std;

void game_throwPropertyError(string propertyName, string typeName)
{
	cout << "[ERR] Cannot set property '" << propertyName << "'. Expecting type '" << typeName << "'!" << endl;
}
