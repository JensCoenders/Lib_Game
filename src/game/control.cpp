#include <SDL_image.h>
#include "control.h"

using namespace std;

/* Shared Memory */

bool Game_SharedMemory::p_running = false;
bool Game_SharedMemory::p_useFPSCounter = false;
float Game_SharedMemory::p_zoomScale = 0.0;
bool Game_SharedMemory::r_SDLInitialized = false;
Game_Layer* Game_SharedMemory::r_layers = new Game_Layer[GAME_LAYER_AMOUNT];
SDL_Window* Game_SharedMemory::r_window = NULL;
SDL_Renderer* Game_SharedMemory::r_windowRenderer = NULL;

/* Extra control functions */

// Rendering
void game_mainThread();

// Events
void game_processKeyboardEvent(SDL_Event* event);
void game_processMouseEvent(SDL_Event* event);
void game_processWindowEvent(SDL_Event* event);


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

	if (Game_SharedMemory::r_SDLInitialized)
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

	Game_SharedMemory::r_window = window;
	Game_SharedMemory::r_windowRenderer = windowRenderer;
	Game_SharedMemory::r_SDLInitialized = true;
	return result;
}

void game_destroySDL()
{
	// Destroy renderer and window
	SDL_DestroyRenderer(Game_SharedMemory::r_windowRenderer);
	SDL_DestroyWindow(Game_SharedMemory::r_window);

	// Quit SDL and SDL image
	SDL_Quit();
	IMG_Quit();

	Game_SharedMemory::r_window = NULL;
	Game_SharedMemory::r_windowRenderer = NULL;
	Game_SharedMemory::r_SDLInitialized = false;
}

// Rendering
void game_startMainThread()
{
	// TODO: Initialize thread
	Game_SharedMemory::p_running = true;
	game_mainThread();
}

void game_stopMainThread()
{
	// TODO: Kill main thread
	Game_SharedMemory::p_running = false;
}

void game_mainThread()
{
	while (Game_SharedMemory::p_running)
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
			Game_Layer* currentLayer = &Game_SharedMemory::r_layers[i];
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
