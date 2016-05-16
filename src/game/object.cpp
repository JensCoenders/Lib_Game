#include <iostream>
#include <sstream>

#include "object.h"

using namespace std;

string game_bounds::toString()
{
	ostringstream stringStream;
	stringStream << "{ (" << worldX << ", " << worldY << "), (" << worldWidth << ", " << worldHeight << ") }";
	return stringStream.str();
}

void Game_Object::setBounds(Game_Bounds bounds)
{
	if (bounds.worldX < 0 || bounds.worldY < 0 || bounds.worldWidth < 1 || bounds.worldHeight < 1)
	{
		cout << "[WARN] Invalid bounds provided " << bounds.toString() << endl;
	}
	else
	{
		m_bounds = bounds;
	}
}

SDL_Texture* Game_Object::render(SDL_Surface* surface, SDL_Renderer* softwareRenderer)
{
	return NULL;
}

Game_Object::Game_Object(int worldX, int worldY, int worldWidth, int worldHeight)
{
	// ID
	static int lastID = 0;
	m_ID = lastID++;

	// Bounds
	m_bounds.worldX = worldX;
	m_bounds.worldY = worldY;
	m_bounds.worldWidth = worldWidth;
	m_bounds.worldHeight = worldHeight;
}
