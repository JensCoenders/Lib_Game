#include <iostream>

#include "shared.h"
#include "object.h"

using namespace std;

game_renderlayer::game_renderlayer()
{
	objectCount = 0;
	objectList = NULL;
}

game_renderlayer::~game_renderlayer()
{
	delete objectList;   // Deleting the first object node will destroy the whole linked list
}

unsigned int Game_Object::getID()
{
	return m_ID;
}

Game_RenderPars* Game_Object::getRenderPars()
{
	return m_renderPars;
}

void Game_Object::setRenderPars(Game_RenderPars* renderPars)
{
	if (m_renderPars)
	{
		delete m_renderPars;
		m_renderPars = NULL;
	}

	m_renderPars = renderPars;
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
	if (m_textureUpdateFunc)
		return m_textureUpdateFunc(*this, equipment);

	return NULL;
}

void Game_Object::setFrameUpdate(Game_ObjectFUFunc function)
{
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

Game_Object::Game_Object(int x, int y, int w, int h, bool staticObject)
{
	isStatic = staticObject;

	lastRenderedTexture = NULL;

	worldCoords.x = x;
	worldCoords.y = y;
	worldSize.width = w;
	worldSize.height = h;

	m_needsTextureUpdate = true;

	m_frameUpdateFunc = NULL;
	m_textureUpdateFunc = NULL;

	m_properties = NULL;

	m_renderPars = NULL;

	static unsigned int lastID = 0;
	m_ID = lastID++;
}

Game_Object::~Game_Object()
{
	if (m_renderPars)
		delete m_renderPars;
}

string Game_TextObject::getText()
{
	return m_text;
}

SDL_Color Game_TextObject::getTextColor()
{
	return m_textColor;
}

void Game_TextObject::setText(string text)
{
	m_text = text;
	requestTextureUpdate();
}

void Game_TextObject::setTextColor(SDL_Color color)
{
	m_textColor = color;
	requestTextureUpdate();
}

SDL_Surface* Game_TextObject::renderText()
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
	{
		// TODO: Fix ugly text font due to scaling
		textObject.worldSize.width = textSurface->w * textObject.textScaling;
		textObject.worldSize.height = textSurface->h * textObject.textScaling;
	}

	return textSurface;
}

Game_TextObject::Game_TextObject(int x, int y, int w, int h, bool isStatic) :
		Game_Object(x, y, w, h, isStatic)
{
	textScaling = 1.0;
	autoSize = true;

	m_text = "";

	m_textColor.r = 0;
	m_textColor.g = 0;
	m_textColor.b = 0;
	m_textColor.a = 255;

	setTextureUpdate(textObjectTextureUpdate);
}

bool Game_AdvancedObject::callEventFunction(Game_ObjectEventType type, Game_ObjectEvent& event)
{
	switch (type)
	{
		case EVENT_TYPE_TYPED:
			if (keyTypedFunc)
				keyTypedFunc(*this, event);
			else
				return false;

			break;
		case EVENT_TYPE_CLICKED:
			if (mouseClickedFunc)
				mouseClickedFunc(*this, (Game_MouseClickedEvent&) event);
			else
				return false;

			break;
		default:
			return false;
	}
	return true;
}

int Game_AdvancedObject::getIntProperty(string name, int defaultValue)
{
	LinkedListNode<Game_ObjectProperty>* currentProperty = findPropertyByName(name);

	// Return default value if property was not found
	if (!currentProperty)
		return defaultValue;

	return currentProperty->value->getIntValue();
}

bool Game_AdvancedObject::getBoolProperty(string name, bool defaultValue)
{
	LinkedListNode<Game_ObjectProperty>* currentProperty = findPropertyByName(name);

	// Return default value if property was not found
	if (!currentProperty)
		return defaultValue;

	return currentProperty->value->getBoolValue();
}

string Game_AdvancedObject::getStringProperty(string name, string defaultValue)
{
	LinkedListNode<Game_ObjectProperty>* currentProperty = findPropertyByName(name);

	// Return default value if property was not found
	if (!currentProperty)
		return defaultValue;

	return currentProperty->value->getStringValue();
}

LinkedListNode<Game_ObjectProperty>* Game_AdvancedObject::findPropertyByName(string name)
{
	// Convert name to lowercase
	for (unsigned int i = 0; i < name.length(); i++)
		name[i] = tolower(name.at(i));

	// Get matching property
	LinkedListNode<Game_ObjectProperty>* currentNode = m_properties;
	while (currentNode)
	{
		if (currentNode->value->name == name)
			return currentNode;

		currentNode = currentNode->nextNode;
	}

	return NULL;
}

Game_AdvancedObject::Game_AdvancedObject(int x, int y, int w, int h, bool isStatic) :
		Game_TextObject(x, y, w, h, isStatic)
{
	keyTypedFunc = NULL;
	mouseClickedFunc = NULL;
}

Game_AdvancedObject::~Game_AdvancedObject()
{
	LinkedListNode<Game_ObjectProperty>* currentNode = m_properties;
	LinkedListNode<Game_ObjectProperty>* tempNode = NULL;
	while (currentNode)
	{
		tempNode = currentNode->nextNode;
		delete currentNode;
		currentNode = tempNode;
	}
}
