#include <iostream>
#include <SDL_ttf.h>
#include "object.h"
#include "shared.h"

using namespace std;

game_objectnode::game_objectnode()
{
	prevNode = NULL;
	nextNode = NULL;
	object = NULL;
}

game_objectnode::~game_objectnode()
{
	if (prevNode != NULL)
		delete prevNode;

	if (nextNode != NULL)
		delete nextNode;
}

game_layer::game_layer()
{
	objectCount = 0;
	objectList = NULL;
}

game_layer::~game_layer()
{
	delete objectList;   // Deleting the first object node will destroy the whole linked list
}

Game_Point Game_Object::getCoords()
{
	return m_coords;
}

void Game_Object::setCoords(int x, int y)
{
	m_coords.x = x;
	m_coords.y = y;
}

Game_Rect Game_Object::getSize()
{
	return m_size;
}

void Game_Object::setSize(int width, int height)
{
	if (width > 0)
	{
		m_size.width = width;
		m_textureSize.width = width;
	}

	if (height > 0)
	{
		m_size.height = height;
		m_textureSize.height = height;
	}
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

void Game_Object::textureUpdate(SDL_Surface* surface, SDL_Renderer* renderer)
{
	if (!m_textureUpdateFunc)
		return;

	m_textureUpdateFunc(this, surface, renderer);
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
		case EVENT_TYPE_KEY:
			if (m_keyTypedFunc)
				m_keyTypedFunc(this, event);
			else
				return false;

			break;
		case EVENT_TYPE_MOUSE:
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
		case EVENT_TYPE_KEY:
			m_keyTypedFunc = function;
			break;
		case EVENT_TYPE_MOUSE:
			m_mouseClickedFunc = function;
			break;
	}
}

int Game_Object::getIntProperty(string name, int defaultValue)
{
	Game_ObjectProperty* currentProperty = findPropertyByName(name);

	// Return default value if property was not found
	if (!currentProperty || !currentProperty->m_intValue)
		return defaultValue;

	return *currentProperty->m_intValue;
}

bool Game_Object::getBoolProperty(string name, bool defaultValue)
{
	Game_ObjectProperty* currentProperty = findPropertyByName(name);

	// Return default value if property was not found
	if (!currentProperty || !currentProperty->m_boolValue)
		return defaultValue;

	return *currentProperty->m_boolValue;
}

string Game_Object::getStringProperty(string name, string defaultValue)
{
	Game_ObjectProperty* currentProperty = findPropertyByName(name);

	// Return default value if property was not found
	if (!currentProperty || !currentProperty->m_stringValue)
		return defaultValue;

	return *currentProperty->m_stringValue;
}

unsigned int Game_Object::getID()
{
	return m_ID;
}

Game_ObjectType Game_Object::getType()
{
	return m_objectType;
}

Game_ObjectProperty* Game_Object::findPropertyByName(string name)
{
	// Convert name to lowercase
	for (unsigned int i = 0; i < name.length(); i++)
		name[i] = tolower(name.at(i));

	// Get matching property
	Game_ObjectProperty* currentProperty = m_properties;
	while (currentProperty)
	{
		if (currentProperty->m_name == name)
			return currentProperty;

		currentProperty = currentProperty->m_nextProperty;
	}

	return NULL;
}

Game_Object::Game_Object(int x, int y, int w, int h)
{
	m_needsTextureUpdate = true;
	m_textureSize.width = 0;
	m_textureSize.height = 0;
	m_lastRenderedTexture = NULL;

	m_frameUpdateFunc = NULL;
	m_textureUpdateFunc = NULL;

	m_keyTypedFunc = NULL;
	m_mouseClickedFunc = NULL;

	m_properties = NULL;

	static unsigned int lastID = 0;
	m_ID = lastID++;

	m_objectType = OBJECT_TYPE_NORMAL;

	setCoords(x, y);
	setSize(w, h);
}

Game_Object::~Game_Object()
{
	Game_ObjectProperty* currentProperty = m_properties;
	Game_ObjectProperty* tempProperty = NULL;
	while (currentProperty)
	{
		tempProperty = currentProperty->m_nextProperty;
		delete currentProperty;
		currentProperty = tempProperty;
	}
}

string Game_GUIObject::getText()
{
	return m_text;
}

void Game_GUIObject::setText(string text)
{
	m_text = text;
	requestTextureUpdate();
}

SDL_Color Game_GUIObject::getTextColor()
{
	return m_textColor;
}

void Game_GUIObject::setTextColor(SDL_Color color)
{
	m_textColor = color;
	requestTextureUpdate();
}

SDL_Color Game_GUIObject::getBackgroundColor()
{
	return m_backgroundColor;
}

void Game_GUIObject::setBackgroundColor(SDL_Color color)
{
	m_backgroundColor = color;
	requestTextureUpdate();
}

void worldObjectTextureUpdate(Game_Object* object, SDL_Surface* surface, SDL_Renderer* renderer)
{
	Game_GUIObject* guiObject = (Game_GUIObject*) object;

	// Clear background surface
	SDL_Color backgroundColor = guiObject->getBackgroundColor();
	SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, backgroundColor.a);
	SDL_RenderClear(renderer);

	// Create text
	SDL_Color textColor = guiObject->getTextColor();
	SDL_Surface* textSurface = TTF_RenderText_Blended(Game_SharedMemory::m_guiFont, guiObject->getText().c_str(), textColor);

	SDL_Rect destRect = {0, 0, textSurface->w, textSurface->h};
	SDL_BlitSurface(textSurface, NULL, surface, &destRect);

	// Free resources
	SDL_FreeSurface(textSurface);
}

Game_GUIObject::Game_GUIObject(int x, int y, int w, int h) :
		Game_Object(x, y, w, h)
{
	m_text = "";
	m_objectType = OBJECT_TYPE_GUI;

	m_textColor.r = 255;
	m_textColor.g = 255;
	m_textColor.b = 255;
	m_textColor.a = 0;
	m_backgroundColor.r = 0;
	m_backgroundColor.g = 0;
	m_backgroundColor.b = 0;
	m_backgroundColor.a = 0;

	setTextureUpdate(worldObjectTextureUpdate);
}

Game_WorldObject::Game_WorldObject(int worldX, int worldY, int worldWidth, int worldHeight) :
		Game_Object(worldX, worldY, worldWidth, worldHeight)
{
	m_objectType = OBJECT_TYPE_WORLD;
}
