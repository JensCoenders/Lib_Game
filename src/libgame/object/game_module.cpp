#include "game_object.h"
#include "game_tools.h"
#include "game_property.h"

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

SDL_Surface* colorBackgroundTU(Game_Object& object, Game_RenderEquipment* equipment)
{
	if (!object.isModuleEnabled(MODULE_COLOR_BACKGROUND))
		return NULL;

	SDL_Color bg = object.colorBackgroundModule->backgroundColor;
	SDL_SetRenderDrawColor(equipment->softwareRenderer, bg.r, bg.g, bg.b, bg.a);

	SDL_Rect rect = {0, 0, object.size.width, object.size.height};
	SDL_RenderFillRect(equipment->softwareRenderer, &rect);

	return equipment->surface;
}

Game_ModuleColorBackground::Game_ModuleColorBackground(Game_Object* parent) :
		Game_Module(parent)
{
	backgroundColor.r = 255;
	backgroundColor.g = 255;
	backgroundColor.b = 255;
	backgroundColor.a = 0;
}

bool Game_ModuleEvent::callEventFunction(Game_ObjectEventType type, Game_ObjectEvent& event)
{
	switch (type)
	{
		case EVENT_TYPE_TYPED:
			if (keyTypedFunc)
				keyTypedFunc(*getParent(), event);
			else
				return false;

			break;
		case EVENT_TYPE_CLICKED:
			if (mouseClickedFunc)
				mouseClickedFunc(*getParent(), (Game_MouseClickedEvent&) event);
			else
				return false;

			break;
		default:
			return false;
	}
	return true;
}

Game_ModuleEvent::Game_ModuleEvent(Game_Object* parent) :
		Game_Module(parent)
{
	keyTypedFunc = NULL;
	mouseClickedFunc = NULL;
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
	LinkedListNode<Game_ObjectProperty>* currentProperty = findPropertyByName(name);

	// Return default value if property was not found
	if (!currentProperty)
		return defaultValue;

	return currentProperty->value->getIntValue();
}

bool Game_ModuleProperty::getBoolProperty(string name, bool defaultValue)
{
	LinkedListNode<Game_ObjectProperty>* currentProperty = findPropertyByName(name);

	// Return default value if property was not found
	if (!currentProperty)
		return defaultValue;

	return currentProperty->value->getBoolValue();
}

string Game_ModuleProperty::getStringProperty(string name, string defaultValue)
{
	LinkedListNode<Game_ObjectProperty>* currentProperty = findPropertyByName(name);

	// Return default value if property was not found
	if (!currentProperty)
		return defaultValue;

	return currentProperty->value->getStringValue();
}

LinkedListNode<Game_ObjectProperty>* Game_ModuleProperty::findPropertyByName(string name)
{
	// Convert name to lowercase
	for (unsigned int i = 0; i < name.length(); i++)
		name[i] = tolower(name.at(i));

	// Get matching property
	LinkedListNode<Game_ObjectProperty>* currentNode = propertyList;
	while (currentNode)
	{
		if (currentNode->value->name == name)
			return currentNode;

		currentNode = currentNode->nextNode;
	}

	return NULL;
}

Game_ModuleProperty::Game_ModuleProperty(Game_Object* parent) :
		Game_Module(parent)
{
	propertyList = NULL;
}

Game_ModuleProperty::~Game_ModuleProperty()
{

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
	if (m_text == "")
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
