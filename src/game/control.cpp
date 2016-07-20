#include <SDL_image.h>
#include "control.h"

using namespace std;

/* Extra control functions */

// Rendering
void game_mainThread();

// Events
void game_processKeyboardEvent(SDL_Event* event);
void game_processMouseEvent(SDL_Event* event);
void game_processWindowEvent(SDL_Event* event);

/* Shared Memory */
game_sharedmemory_t game_sharedMemory;

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


/* Control functions */

// SDL
Game_Result game_initializeSDL(string windowTitle)
{
	Game_Result result = {GAME_SUCCESS, ""};

	if (game_sharedMemory.r_SDLInitialized)
	{
		result.returnCode = GAME_ERR_ALREADY_INIT;
		return result;
	}

	// Initialize SDL
	int flags = SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER;
	if (SDL_Init(flags) == -1)
	{
		result.returnCode = GAME_CRIT_SDL;
		result.message = SDL_GetError();
		return result;
	}

	// Initialize SDL image
	flags = IMG_INIT_PNG;
	if ((IMG_Init(flags) & flags) != flags)
	{
		result.returnCode = GAME_CRIT_SDL_IMG;
		result.message = SDL_GetError();
		return result;
	}

	// Setup window
	flags = SDL_WINDOW_RESIZABLE;
	SDL_Window* window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 576, flags);

	// Setup renderer
	flags = SDL_RENDERER_ACCELERATED;
	SDL_Renderer* windowRenderer = SDL_CreateRenderer(window, -1, flags);

	game_sharedMemory.r_window = window;
	game_sharedMemory.r_windowRenderer = windowRenderer;
	game_sharedMemory.r_SDLInitialized = true;
	return result;
}

void game_destroySDL()
{
	// Destroy renderer and window
	SDL_DestroyRenderer(game_sharedMemory.r_windowRenderer);
	SDL_DestroyWindow(game_sharedMemory.r_window);

	// Quit SDL and SDL image
	SDL_Quit();
	IMG_Quit();

	game_sharedMemory.r_window = NULL;
	game_sharedMemory.r_windowRenderer = NULL;
	game_sharedMemory.r_SDLInitialized = false;
}

// Rendering
void game_startMainThread()
{
	// TODO: Initialize thread
	game_sharedMemory.p_running = true;
	game_mainThread();
}

void game_stopMainThread()
{
	// TODO: Kill main thread
	game_sharedMemory.p_running = false;
}

void game_mainThread()
{
	while (game_sharedMemory.p_running)
	{
		// Poll events
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYUP:
				case SDL_KEYDOWN:
					game_processKeyboardEvent(&event);
					break;
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEMOTION:
					game_processMouseEvent(&event);
					break;
				case SDL_WINDOWEVENT:
					game_processWindowEvent(&event);
					break;
			}
		}

		// Redraw all objects
		for (int i = 0; i < GAME_LAYER_AMOUNT; i++)
		{
			Game_Layer* currentLayer = &game_sharedMemory.r_layers[i];
			Game_ObjectNode* currentObjectNode = currentLayer->objectList;
			while (currentObjectNode != NULL)
			{
				// TODO: Draw object
				currentObjectNode = currentObjectNode->nextNode;
			}
		}
	}
}

void game_processKeyboardEvent(SDL_Event* event)
{

}

void game_processMouseEvent(SDL_Event* event)
{

}

void game_processWindowEvent(SDL_Event* event)
{
	switch (event->window.event)
	{
		case SDL_WINDOWEVENT_CLOSE:
			game_stopMainThread();
			break;
	}
}
