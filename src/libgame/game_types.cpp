#include <iostream>
#include <sstream>
#include "game_types.h"

using namespace std;

string Game_Point::toString()
{
	ostringstream stringStream;
	stringStream << "(x, y) = (" << x << ", " << y << ")";
	return stringStream.str();
}

string Game_Rect::toString()
{
	ostringstream stringStream;
	stringStream << "(width, height) = (" << width << ", " << height << ")";
	return stringStream.str();
}

Game_ObjectEvent::Game_ObjectEvent(SDL_Event* event)
{
	this->originalEvent = event;
}

Game_MouseClickedEvent::Game_MouseClickedEvent(SDL_Event* event) :
		Game_ObjectEvent(event)
{
	this->button = event->button.button;
	this->pressed = (event->button.state == SDL_PRESSED);
	this->relX = 0;
	this->relY = 0;
}

Game_RenderPars::Game_RenderPars(Game_RenderMode renderMode)
{
	this->renderMode = renderMode;
}

Game_RenderPars::~Game_RenderPars()
{
}

Game_RP_StaticBackground::Game_RP_StaticBackground(SDL_Color backgroundColor) :
		Game_RenderPars(RENDERMODE_STATIC_BACKGROUND)
{
	this->backgroundColor = backgroundColor;
}

Game_RP_ImageTexture::Game_RP_ImageTexture(string imageTexturePath) :
		Game_RenderPars(RENDERMODE_IMAGE_TEXTURE)
{
	this->imageTexturePath = imageTexturePath;
}

Game_RenderEquipment::Game_RenderEquipment(SDL_Renderer* renderer, SDL_Surface* surface)
{
	this->softwareRenderer = renderer;
	this->surface = surface;
}

Game_RenderEquipment::~Game_RenderEquipment()
{
	if (surface)
		SDL_FreeSurface(surface);

	if (softwareRenderer)
		SDL_DestroyRenderer(softwareRenderer);
}

int Game_ObjectProperty::getIntValue()
{
	return m_intValue;
}

bool Game_ObjectProperty::getBoolValue()
{
	return m_boolValue;
}

string Game_ObjectProperty::getStringValue()
{
	return *m_stringValue;
}

Game_ObjectProperty::Game_ObjectProperty()
{
	m_intValue = 0;
	m_boolValue = false;
	m_stringValue = NULL;
}

Game_ObjectProperty::~Game_ObjectProperty()
{
	if (m_stringValue)
		delete m_stringValue;
}
