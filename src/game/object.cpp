#include "object.h"

using namespace std;

bool Game_Object::render(SDL_Surface* surface, SDL_Renderer* softwareRenderer)
{
	return true;
}

Game_Object::Game_Object()
{
	static int lastID = 0;
	m_ID = lastID++;
}
