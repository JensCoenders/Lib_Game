#include <SDL.h>
#include <iostream>
#include <sstream>
#include "types.h"

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

game_renderequipment::game_renderequipment(SDL_Renderer* renderer, SDL_Surface* surface)
{
	this->softwareRenderer = renderer;
	this->surface = surface;
}

game_renderequipment::~game_renderequipment()
{
	if (surface)
		SDL_FreeSurface(surface);

	if (softwareRenderer)
		SDL_DestroyRenderer(softwareRenderer);
}
