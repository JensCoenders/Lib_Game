#include <iostream>
#include <SDL_image.h>

#include "controller.h"

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
	delete objectList;	// Deleting the first object node will destroy the whole linked list
}

Game_ErrorMsg Game_Controller::initializeSDL(string windowTitle)
{
	Game_ErrorMsg result = {GAME_SUCCESS, ""};

	if (m_SDLInitialized)
	{
		result.errorCode = GAME_ERR_ALREADY_INIT;
		return result;
	}

	// Initialize SDL
	int flags = SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER;
	if (SDL_Init(flags) == -1)
	{
		result.errorCode = GAME_CRIT_SDL;
		result.message = SDL_GetError();
		return result;
	}

	// Initialize SDL image
	flags = IMG_INIT_PNG;
	if ((IMG_Init(flags) & flags) != flags)
	{
		result.errorCode = GAME_CRIT_SDL_IMG;
		result.message = SDL_GetError();
		return result;
	}

	// Setup window
	flags = SDL_WINDOW_RESIZABLE;
	SDL_Window* window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 576, flags);

	// Setup renderer
	flags = SDL_RENDERER_ACCELERATED;
	SDL_Renderer* windowRenderer = SDL_CreateRenderer(window, -1, flags);

	m_window = window;
	m_windowRenderer = windowRenderer;
	m_SDLInitialized = true;
	return result;
}

void Game_Controller::destroySDL()
{
	m_running = false;

	// Destroy renderer and window
	SDL_DestroyRenderer(m_windowRenderer);
	SDL_DestroyWindow(m_window);

	// Quit SDL and SDL image
	SDL_Quit();
	IMG_Quit();

	m_window = NULL;
	m_windowRenderer = NULL;
	m_SDLInitialized = false;
}

void Game_Controller::gameLoop()
{
	while (m_running)
	{
		// Poll events
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			// Select type and call event process function
			switch (event.type)
			{
				case SDL_KEYUP:
				case SDL_KEYDOWN:
					processKeyboardEvent(&event);
					break;
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEMOTION:
					processMouseEvent(&event);
					break;
				case SDL_WINDOWEVENT:
					processWindowEvent(&event);
					break;
			}
		}

		// Redraw all objects
		Game_Layer* currentLayer = NULL;
		Game_ObjectNode* currentObjectNode = NULL;
		for (int i = 0; i < GAME_LAYER_AMOUNT; i++)
		{
			currentLayer = &m_layers[i];
			currentObjectNode = currentLayer->objectList;

			do
			{

				currentObjectNode = currentObjectNode->nextNode;
			}
			while (currentObjectNode != NULL);
		}
	}
}

Game_Controller::Game_Controller()
{
	m_running = true;
	m_layers = new Game_Layer[GAME_LAYER_AMOUNT];

	m_SDLInitialized = false;
	m_window = NULL;
	m_windowRenderer = NULL;
	m_zoomScale = 0.0;
}

Game_Controller::~Game_Controller()
{
	if (m_SDLInitialized)
		destroySDL();

	delete[] m_layers;
}

void Game_Controller::processKeyboardEvent(SDL_Event* event)
{

}

void Game_Controller::processMouseEvent(SDL_Event* event)
{

}

void Game_Controller::processWindowEvent(SDL_Event* event)
{

}
