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

game_objectevent::game_objectevent(SDL_Event* event)
{
	this->originalEvent = event;
}

game_mouseclickedeevent::game_mouseclickedeevent(SDL_Event* event) :
		game_objectevent(event)
{
	this->button = event->button.button;
	this->pressed = (event->button.state == SDL_PRESSED);
	this->relX = 0;
	this->relY = 0;
}

game_renderpars::game_renderpars(Game_RenderMode renderMode)
{
	this->renderMode = renderMode;
}

game_rp_staticbackground::game_rp_staticbackground(SDL_Color backgroundColor) :
		game_renderpars(RENDERMODE_STATIC_BACKGROUND)
{
	this->backgroundColor = backgroundColor;
}

game_rp_imagetexture::game_rp_imagetexture(string imageTexturePath) :
		game_renderpars(RENDERMODE_IMAGE_TEXTURE)
{
	this->imageTexturePath = imageTexturePath;
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
