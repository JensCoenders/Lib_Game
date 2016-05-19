#include <iostream>
#include <sstream>

#include "object.h"

using namespace std;

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

void Game_Object::setWorldCoords(Game_Point coords)
{
	if (coords.x >= 0 && coords.y >= 0)
		m_worldCoords = coords;
	else
		cout << "[WARN] Invalid coordinates provided: " << coords.toString() << endl;
}

void Game_Object::setWorldSize(Game_Rect size)
{
	if (size.width > 0 && size.width > 0)
		m_worldSize = size;
	else
		cout << "[WARN] Invalid size provided: " << size.toString() << endl;
}

SDL_Texture* Game_Object::compileTexture(SDL_Surface* surface, SDL_Renderer* softwareRenderer)
{
	// Test rendering
	SDL_RenderClear(softwareRenderer);
	SDL_Rect targetRect = {0, 0, 50, 50};

	SDL_RenderDrawRect(softwareRenderer, &targetRect);
	return SDL_CreateTextureFromSurface(softwareRenderer, surface);
}

void Game_Object::frameUpdate()
{
	// Do nothing...
}

Game_Object::Game_Object(int worldX, int worldY, int worldWidth, int worldHeight)
{
	static int lastID = 0;
	m_ID = lastID++;

	setWorldCoords({worldX, worldY});
	setWorldSize({worldWidth, worldHeight});

	m_needsTextureUpdate = false;
	m_lastRenderedTexture = NULL;
}
