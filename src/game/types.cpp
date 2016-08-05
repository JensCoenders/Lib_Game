#include <sstream>
#include "types.h"

string game_point::toString()
{
	ostringstream stringStream;
	stringStream << "(x, y) = (" << x << ", " << y << ")";
	return stringStream.str();
}

string game_rect::toString()
{
	ostringstream stringStream;
	stringStream << "(width, height) = (" << width << ", " << height << ")";
	return stringStream.str();
}

game_objectproperty::game_objectproperty()
{
	m_name = "";
	m_nextProperty = NULL;

	m_intValue = NULL;
	m_boolValue = NULL;
	m_stringValue = NULL;
}

game_objectproperty::~game_objectproperty()
{
	if (m_intValue)
		delete m_intValue;

	if (m_boolValue)
		delete m_boolValue;

	if (m_stringValue)
		delete m_stringValue;
}
