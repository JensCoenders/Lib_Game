#include <iostream>

#include "game_global.h"
#include "game_object.h"
#include "game_tools.h"

using namespace std;

Game_Object* Game_Module::getParent()
{
	return m_parent;
}

Game_Module::Game_Module(Game_Object* parent)
{
	m_parent = parent;

	if (!m_parent)
	{
		cout << "[ERR]: A module cannot have a NULL parent!" << endl;
		delete this;
	}
}

Game_ModuleColorBackground::Game_ModuleColorBackground(Game_Object* parent) :
		Game_Module(parent)
{
	backgroundColor.r = 255;
	backgroundColor.g = 255;
	backgroundColor.b = 255;
	backgroundColor.a = 0;
}

void Game_ModuleEvent::setEventFunction(Game_ObjectEventType type, Game_ObjectEventFunc function)
{
	Game_ObjectEventFunc* targetFunction = getEventFunctionWithType(type);
	if (targetFunction)
		*targetFunction = function;
}

bool Game_ModuleEvent::callEventFunction(Game_ObjectEventType type, Game_ObjectEvent& event)
{
	Game_ObjectEventFunc targetFunction = *getEventFunctionWithType(type);
	if (!targetFunction)
		return false;

	targetFunction(*getParent(), event);
	return true;
}

Game_ObjectEventFunc* Game_ModuleEvent::getEventFunctionWithType(Game_ObjectEventType type)
{
	switch (type)
	{
		case EVENT_TYPE_TYPED:
			return &keyTypedFunc;
		case EVENT_TYPE_CLICKED:
			return &mouseClickedFunc;
		case EVENT_TYPE_MOTION:
			return &mouseHoverFunc;
		default:
			return NULL;
	}
}

Game_ModuleEvent::Game_ModuleEvent(Game_Object* parent) :
		Game_Module(parent)
{
	keyTypedFunc = NULL;
	mouseClickedFunc = NULL;
	mouseHoverFunc = NULL;

	mouseHovering = false;
}

string Game_ModuleImageBackground::getTexturePath()
{
	return m_texturePath;
}

void Game_ModuleImageBackground::setTexturePath(std::string texturePath)
{
	m_texturePath = texturePath;
	getParent()->requestTextureUpdate();
}

Game_ModuleImageBackground::Game_ModuleImageBackground(Game_Object* parent) :
		Game_Module(parent)
{
	m_texturePath = "";
}

int Game_ModuleProperty::getIntProperty(string name, int defaultValue)
{
	Game_ObjectProperty* currentProperty = propertyList.search(name);
	if (!currentProperty)
		return defaultValue;

	return currentProperty->getIntValue();
}

bool Game_ModuleProperty::getBoolProperty(string name, bool defaultValue)
{
	Game_ObjectProperty* currentProperty = propertyList.search(name);
	if (!currentProperty)
		return defaultValue;

	return currentProperty->getBoolValue();
}

string Game_ModuleProperty::getStringProperty(string name, string defaultValue)
{
	Game_ObjectProperty* currentProperty = propertyList.search(name);
	if (!currentProperty)
		return defaultValue;

	return currentProperty->getStringValue();
}

bool game_modulePropertySearchFunc(Game_ObjectProperty* currentProperty, string name)
{
	for (unsigned int i = 0; i < name.length(); i++)
		name[i] = tolower(name[i]);

	return currentProperty->name == name;
}

Game_ModuleProperty::Game_ModuleProperty(Game_Object* parent) :
		Game_Module(parent)
{
	propertyList.setSearchFunc(game_modulePropertySearchFunc);
}

string Game_ModuleText::getText()
{
	return m_text;
}

SDL_Color Game_ModuleText::getTextColor()
{
	return m_textColor;
}

void Game_ModuleText::setText(string text)
{
	m_text = text;
	getParent()->requestTextureUpdate();
}

void Game_ModuleText::setTextColor(SDL_Color color)
{
	m_textColor = color;
	getParent()->requestTextureUpdate();
}

SDL_Surface* Game_ModuleText::renderText()
{
	if (m_text.empty())
		return NULL;

	// Create text
	return TTF_RenderText_Blended(gameVar_guiFont, m_text.c_str(), m_textColor);
}

Game_ModuleText::Game_ModuleText(Game_Object* parent) :
		Game_Module(parent)
{
	autoSize = true;
	textScaling = 1.0f;

	lastRenderSurface = NULL;

	m_text = "";

	m_textColor.r = 0;
	m_textColor.g = 0;
	m_textColor.b = 0;
	m_textColor.a = 0;
}

Game_ModuleExtraBounds::Game_ModuleExtraBounds(Game_Object* parent) :
		Game_Module(parent)
{
	enabled = true;

	marginTop = 0;
	marginRight = 0;
	marginBottom = 0;
	marginLeft = 0;

	paddingTop = 0;
	paddingRight = 0;
	paddingBottom = 0;
	paddingLeft = 0;

	fillScreen = false;
	floatMode = FLOAT_LEFT_TOP;
}
