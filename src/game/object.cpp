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

game_renderlayer::game_renderlayer()
{
	objectCount = 0;
	objectList = NULL;
}

game_renderlayer::~game_renderlayer()
{
	delete objectList;   // Deleting the first object node will destroy the whole linked list
}

SDL_Color Game_Object::getBackgroundColor()
{
	return m_backgroundColor;
}

void Game_Object::setBackgroundColor(SDL_Color color)
{
	m_backgroundColor = color;
	requestTextureUpdate();
}

unsigned int Game_Object::getID()
{
	return m_ID;
}

Game_ObjectType Game_Object::getType()
{
	return m_objectType;
}

void Game_Object::setType(Game_ObjectType type)
{
	m_objectType = type;
}

bool Game_Object::needsTextureUpdate()
{
	return (m_needsTextureUpdate && m_textureUpdateFunc);
}

void Game_Object::requestTextureUpdate()
{
	m_needsTextureUpdate = true;
}

void Game_Object::satisfyTextureUpdate()
{
	m_needsTextureUpdate = false;
}

void Game_Object::frameUpdate()
{
	if (m_frameUpdateFunc)
		m_frameUpdateFunc(*this);
}

SDL_Surface* Game_Object::textureUpdate(Game_RenderEquipment* equipment)
{
	if (!m_textureUpdateFunc)
		return NULL;

	return m_textureUpdateFunc(*this, equipment);
}

void Game_Object::setFrameUpdate(Game_ObjectFUFunc function)
{
	if (!function)
		cout << "[WARN] Attempting to set NULL function as FU function!" << endl;
	else
		m_frameUpdateFunc = function;
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
		m_size.width = width;

	if (height > 0)
		m_size.height = height;
}

Game_Object::Game_Object(int x, int y, int w, int h, Game_ObjectType type)
{
	realCoords.x = 0;
	realCoords.y = 0;
	realSize.width = 0;
	realSize.height = 0;

	lastRenderedTexture = NULL;

	m_needsTextureUpdate = true;

	m_frameUpdateFunc = NULL;
	m_textureUpdateFunc = NULL;

	m_properties = NULL;

	static unsigned int lastID = 0;
	m_ID = lastID++;
	m_objectType = type;

	setCoords(x, y);
	setSize(w, h);
}

bool Game_AdvancedObject::callEventFunction(Game_ObjectEventType type, SDL_Event& event)
{
	switch (type)
	{
		case EVENT_TYPE_TYPED:
			if (m_keyTypedFunc)
				m_keyTypedFunc(*this, event);
			else
				return false;

			break;
		case EVENT_TYPE_CLICKED:
			if (m_mouseClickedFunc)
				m_mouseClickedFunc(*this, event);
			else
				return false;

			break;
		default:
			return false;
	}
	return true;
}

void Game_AdvancedObject::setEventFunction(Game_ObjectEventType type, Game_ObjectEventFunc function)
{
	if (!function)
	{
		cout << "[WARN] Attempting to set NULL function as event function!" << endl;
		return;
	}

	switch (type)
	{
		case EVENT_TYPE_TYPED:
			m_keyTypedFunc = function;
			break;
		case EVENT_TYPE_CLICKED:
			m_mouseClickedFunc = function;
			break;
	}
}

int Game_AdvancedObject::getIntProperty(string name, int defaultValue)
{
	Game_ObjectProperty* currentProperty = findPropertyByName(name);

	// Return default value if property was not found
	if (!currentProperty || !currentProperty->m_intValue)
		return defaultValue;

	return *currentProperty->m_intValue;
}

bool Game_AdvancedObject::getBoolProperty(string name, bool defaultValue)
{
	Game_ObjectProperty* currentProperty = findPropertyByName(name);

	// Return default value if property was not found
	if (!currentProperty || !currentProperty->m_boolValue)
		return defaultValue;

	return *currentProperty->m_boolValue;
}

string Game_AdvancedObject::getStringProperty(string name, string defaultValue)
{
	Game_ObjectProperty* currentProperty = findPropertyByName(name);

	// Return default value if property was not found
	if (!currentProperty || !currentProperty->m_stringValue)
		return defaultValue;

	return *currentProperty->m_stringValue;
}

string Game_AdvancedObject::getText()
{
	return m_text;
}

SDL_Color Game_AdvancedObject::getTextColor()
{
	return m_textColor;
}

void Game_AdvancedObject::setText(string text)
{
	m_text = text;
	requestTextureUpdate();
}

void Game_AdvancedObject::setTextColor(SDL_Color color)
{
	m_textColor = color;
	requestTextureUpdate();
}

SDL_Surface* Game_AdvancedObject::renderText()
{
	if (m_text == "")
		return NULL;

	// Create text
	SDL_Surface* textSurface = TTF_RenderText_Blended(Game_SharedMemory::m_guiFont, m_text.c_str(), m_textColor);
	return textSurface;
}

SDL_Surface* textObjectTextureUpdate(Game_Object& object, Game_RenderEquipment* equipment)
{
	Game_AdvancedObject& textObject = (Game_AdvancedObject&) object;
	SDL_Surface* textSurface = textObject.renderText();

	if (!textSurface)
		return NULL;
	else if (textObject.autoSize)
		// TODO: Fix ugly text font due to scaling
		textObject.setSize(textSurface->w * textObject.textScaling, textSurface->h * textObject.textScaling);

	return textSurface;
}

Game_ObjectProperty* Game_AdvancedObject::findPropertyByName(string name)
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

Game_AdvancedObject::Game_AdvancedObject(int x, int y, int w, int h, Game_ObjectType type) :
		Game_Object(x, y, w, h, type)
{
	m_keyTypedFunc = NULL;
	m_mouseClickedFunc = NULL;

	textScaling = 1.0;
	autoSize = true;

	m_text = "";

	m_textColor.r = 255;
	m_textColor.g = 255;
	m_textColor.b = 255;
	m_textColor.a = 0;

	m_backgroundColor.r = 0;
	m_backgroundColor.g = 0;
	m_backgroundColor.b = 0;
	m_backgroundColor.a = 0;

	setTextureUpdate(textObjectTextureUpdate);
}

Game_AdvancedObject::~Game_AdvancedObject()
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
