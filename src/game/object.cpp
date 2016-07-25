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

SDL_Event* Game_EventObject::pollEvent(SDL_Event** queue)
{
	for (int i = 0; i < GAME_OBJ_MAX_QUEUED_EVENTS; i++)
	{
		if (queue[i] != NULL)
		{
			SDL_Event* event = queue[i];
			queue[i] = NULL;
			return event;
		}
	}

	return NULL;
}

SDL_KeyboardEvent* Game_EventObject::pollKeyboardEvent()
{
	SDL_KeyboardEvent* event = (SDL_KeyboardEvent*) pollEvent((SDL_Event**) m_queuedKeyboardEvents);
	if (event != NULL)
	{
		m_newKeyboardEvents--;
	}

	return event;
}

SDL_MouseButtonEvent* Game_EventObject::pollMouseEvent()
{
	SDL_MouseButtonEvent* event = (SDL_MouseButtonEvent*) pollEvent((SDL_Event**) m_queuedMouseEvents);
	if (event != NULL)
	{
		m_newMouseEvents--;
	}

	return event;
}

void Game_EventObject::addKeyboardEvent(SDL_KeyboardEvent* event)
{
	if (m_newKeyboardEvents < GAME_OBJ_MAX_QUEUED_EVENTS)
	{
		m_queuedKeyboardEvents[m_newKeyboardEvents] = event;
		m_newKeyboardEvents++;
	}
}

void Game_EventObject::addMouseEvent(SDL_MouseButtonEvent* event)
{
	if (m_newMouseEvents < GAME_OBJ_MAX_QUEUED_EVENTS)
	{
		m_queuedMouseEvents[m_newMouseEvents] = event;
		m_newMouseEvents++;
	}
}

Game_Object::Game_Object(int worldX, int worldY, int worldWidth, int worldHeight)
{
	static unsigned int lastID = 0;
	m_ID = lastID++;

	m_worldCoords.x = worldX;
	m_worldCoords.y = worldY;
	m_worldSize.width = worldWidth;
	m_worldSize.height = worldHeight;

	m_needsTextureUpdate = true;
	m_lastRenderedTexture = NULL;
	m_compileTextureFunc = NULL;

	m_frameUpdateFunc = NULL;
}

Game_EventObject::Game_EventObject(int worldX, int worldY, int worldWidth, int worldHeight) :
		Game_Object(worldX, worldY, worldWidth, worldHeight)
{
	m_newKeyboardEvents = 0;
	m_queuedKeyboardEvents = new SDL_KeyboardEvent*[GAME_OBJ_MAX_QUEUED_EVENTS];

	m_newMouseEvents = 0;
	m_queuedMouseEvents = new SDL_MouseButtonEvent*[GAME_OBJ_MAX_QUEUED_EVENTS];
}

Game_EventObject::~Game_EventObject()
{
	delete[] m_queuedKeyboardEvents;
	delete[] m_queuedMouseEvents;
}
