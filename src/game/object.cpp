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

unsigned int Game_Object::getID()
{
	return m_ID;
}

void Game_Object::frameUpdate()
{
	if (m_frameUpdateFunc)
	{
		m_frameUpdateFunc(this);
	}
}

void Game_Object::setFrameUpdate(Game_ObjectFUFunc function)
{
	if (!function)
	{
		cout << "[WARN] Attempting to set NULL function as FU function!" << endl;
		return;
	}

	m_frameUpdateFunc = function;
}

void Game_Object::textureUpdate(Game_Rect bounds, SDL_Renderer* renderer)
{
	if (!m_textureUpdateFunc)
		return;

	m_textureUpdateFunc(this, bounds, renderer);
}

void Game_Object::setTextureUpdate(Game_ObjectTUFunc function)
{
	if (!function)
	{
		cout << "[WARN] Attempting to set NULL function as TU function!" << endl;
		return;
	}

	m_textureUpdateFunc = function;
}

void Game_Object::requestTextureUpdate()
{
	m_needsTextureUpdate = true;
}

bool Game_Object::needsTextureUpdate()
{
	return (m_needsTextureUpdate && m_textureUpdateFunc);
}

bool Game_Object::callEventFunction(Game_ObjectEventType type, SDL_Event* event)
{
	switch (type)
	{
		case TYPE_KEY_TYPED:
			if (m_keyTypedFunc)
				m_keyTypedFunc(this, event);
			else
				return false;

			break;
		case TYPE_MOUSE_CLICKED:
			if (m_mouseClickedFunc)
				m_mouseClickedFunc(this, event);
			else
				return false;

			break;

		default:
			return false;
	}
	return true;
}

void Game_Object::setEventFunction(Game_ObjectEventType type, Game_ObjectEventFunc function)
{
	if (!function)
	{
		cout << "[WARN] Attempting to set NULL function as event function!" << endl;
		return;
	}

	switch (type)
	{
		case TYPE_KEY_TYPED:
			m_keyTypedFunc = function;
			break;
		case TYPE_MOUSE_CLICKED:
			m_mouseClickedFunc = function;
			break;
	}
}

Game_Object::Game_Object(int worldX, int worldY, int worldWidth, int worldHeight)
{
	m_worldCoords.x = worldX;
	m_worldCoords.y = worldY;
	m_worldSize.width = worldWidth;
	m_worldSize.height = worldHeight;

	m_needsTextureUpdate = true;
	m_lastRenderedTexture = NULL;

	m_frameUpdateFunc = NULL;
	m_textureUpdateFunc = NULL;

	m_keyTypedFunc = NULL;
	m_mouseClickedFunc = NULL;

	m_integerProperties = NULL;
	m_stringProperties = NULL;

	static unsigned int lastID = 0;
	m_ID = lastID++;
}

Game_Object::~Game_Object()
{
	Game_ObjectProperty<string>* currentStringProperty = m_stringProperties;
	Game_ObjectProperty<string>* tempStringProperty = NULL;
	while (currentStringProperty)
	{
		tempStringProperty = currentStringProperty->nextProperty;
		delete currentStringProperty;
		currentStringProperty = tempStringProperty;
	}

	Game_ObjectProperty<int>* currentIntegerProperty = m_integerProperties;
	Game_ObjectProperty<int>* tempIntegerProperty = NULL;
	while (currentIntegerProperty)
	{
		tempIntegerProperty = currentIntegerProperty->nextProperty;
		delete currentIntegerProperty;
		currentIntegerProperty = tempIntegerProperty;
	}
}
