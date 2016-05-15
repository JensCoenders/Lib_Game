#include <iostream>
#include <SDL_image.h>
#include "controller.h"

using namespace std;

Game_ErrorMsg Game_Controller::initializeSDL(string windowTitle)
{
	Game_ErrorMsg result = { GAME_SUCCESS, "" };

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

}

Game_Controller::Game_Controller()
{
	m_SDLInitialized = false;
	m_window = NULL;
	m_windowRenderer = NULL;
}

Game_Controller::~Game_Controller()
{
	if (m_SDLInitialized)
	{
		destroySDL();
	}

	// TODO: Complete Game_Controller destructor
}
