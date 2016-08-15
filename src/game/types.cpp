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

int game_objectproperty::getIntValue()
{
	return m_intValue;
}

bool game_objectproperty::getBoolValue()
{
	return m_boolValue;
}

string game_objectproperty::getStringValue()
{
	return *m_stringValue;
}

game_objectproperty::game_objectproperty()
{
	m_intValue = 0;
	m_boolValue = false;
	m_stringValue = NULL;
}

game_objectproperty::~game_objectproperty()
{
	if (m_stringValue)
		delete m_stringValue;
}
