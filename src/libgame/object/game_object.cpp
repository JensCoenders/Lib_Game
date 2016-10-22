#include <iostream>

#include "game_object.h"

using namespace std;

bool Game_Object::isModuleEnabled(Game_ModuleType module)
{
	return m_enabledModules & module;
}

void Game_Object::setModuleEnabled(Game_ModuleType module, bool enabled)
{
	if (module & MODULE_COLOR_BACKGROUND)
	{
		if (enabled)
			colorBackgroundModule = new Game_ModuleColorBackground(this);
		else
			delete colorBackgroundModule;
	}
	if (module & MODULE_EVENT)
	{
		if (enabled)
			eventModule = new Game_ModuleEvent(this);
		else
			delete eventModule;
	}
	if (module & MODULE_IMAGE_BACKGROUND)
	{
		if (enabled)
			imageBackgroundModule = new Game_ModuleImageBackground(this);
		else
			delete imageBackgroundModule;
	}
	if (module & MODULE_PROPERTY)
	{
		if (enabled)
			propertyModule = new Game_ModuleProperty(this);
		else
			delete propertyModule;
	}
	if (module & MODULE_TEXT)
	{
		if (enabled)
			textModule = new Game_ModuleText(this);
		else
			delete textModule;
	}
	if (module & MODULE_EXTRA_BOUNDS)
	{
		if (enabled)
			extraBoundsModule = new Game_ModuleExtraBounds(this);
		else
			delete extraBoundsModule;
	}

	if (enabled)
		m_enabledModules |= module;
	else
		m_enabledModules &= ~module;
}

void Game_Object::runFrameUpdate()
{
	if (m_frameUpdateFunc)
		m_frameUpdateFunc(*this);
}

void Game_Object::setFrameUpdate(Game_ObjectFUFunc function)
{
	m_frameUpdateFunc = function;
}

SDL_Surface* Game_Object::runTextureUpdate(Game_RenderEquipment& equipment)
{
	if (m_textureUpdateFunc)
		return m_textureUpdateFunc(*this, equipment);

	return NULL;
}

void Game_Object::setTextureUpdate(Game_ObjectTUFunc function)
{
	m_textureUpdateFunc = function;
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

unsigned int Game_Object::getID()
{
	return m_ID;
}

Game_Object::Game_Object(int x, int y, int w, int h)
{
	colorBackgroundModule = NULL;
	eventModule = NULL;
	imageBackgroundModule = NULL;
	propertyModule = NULL;
	textModule = NULL;
	extraBoundsModule = NULL;

	lastRenderedTexture = NULL;
	isOutsideCameraBounds = false;

	position.x = x;
	position.y = y;
	size.width = w;
	size.height = h;
	rotation = 0.0;

	isStatic = false;
	isVisible = true;

	m_needsTextureUpdate = true;

	m_enabledModules = 0;

	m_frameUpdateFunc = NULL;
	m_textureUpdateFunc = NULL;

	static unsigned int lastID = 0;
	m_ID = lastID++;
}

Game_Object::Game_Object(int x, int y, int w, int h, bool staticObject) :
		Game_Object(x, y, w, h)
{
	isStatic = staticObject;
}

Game_Object::Game_Object(int x, int y, int w, int h, Game_ModuleType enabledModules) :
		Game_Object(x, y, w, h)
{
	setModuleEnabled(enabledModules, true);
}

Game_Object::Game_Object(int x, int y, int w, int h, bool staticObject, Game_ModuleType enabledModules) :
		Game_Object(x, y, w, h)
{
	isStatic = staticObject;
	setModuleEnabled(enabledModules, true);
}

Game_Object::~Game_Object()
{
	// Free memory for all modules
	setModuleEnabled(MODULE_ALL, false);
}

Game_RenderLayer::Game_RenderLayer()
{
	objectCount = 0;
	objectList = NULL;
}

Game_RenderLayer::~Game_RenderLayer()
{
	delete objectList;  	 // Deleting the first object node will destroy the whole linked list
}
