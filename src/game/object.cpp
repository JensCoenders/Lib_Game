#include "object.h"

using namespace std;

Game_Object::Game_Object()
{
	static int lastID = 0;
	m_ID = lastID++;
}
