#include <iostream>
#include <time.h>
#include <sstream>
#include "control.h"
#include "thread.h"

using namespace std;

void game_renderThread();

void game_mainLoop()
{
	// Start render thread
	game_startRenderThread();

	while (Game_SharedMemory::p_running)
	{
		// Wait and poll events
		SDL_Event event;
		if (SDL_WaitEvent(&event))
		{
			switch (event.type)
			{
				case SDL_KEYUP:
				case SDL_KEYDOWN:
					game_processKeyboardEvent(event);
					break;
				case SDL_MOUSEBUTTONDOWN:
					game_processMouseEvent(event);
					break;
				case SDL_WINDOWEVENT:
					game_processWindowEvent(event);
					break;
			}
		}
	}

	// Join render thread
	game_joinRenderThread();
}

void game_startRenderThread()
{
	Game_SharedMemory::p_running = true;

	// Start thread
	Game_SharedMemory::r_renderThreadID = game_startThread(game_renderThread);
	if (Game_SharedMemory::r_renderThreadID < 0)
		cout << "[ERR] Couldn't start render thread! Error code: " << Game_SharedMemory::r_renderThreadID << endl;
	else
		cout << "[INFO] Started render thread with ID " << Game_SharedMemory::r_renderThreadID << endl;
}

void game_joinRenderThread()
{
	game_joinThread(Game_SharedMemory::r_renderThreadID);
}

void game_renderThread()
{
	// Setup renderer
	SDL_Renderer* mainRenderer = Game_SharedMemory::s_mainRenderer;
	SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 255);

	// Setup FPS object
	Game_GUIObject* fpsObject = new Game_GUIObject(0, 0, 100, 50);
	fpsObject->setText("FPS: 0");
	fpsObject->setTextColor( {255, 255, 255, 0});

	Game_SharedMemory::m_fpsObject = fpsObject;
	Game_SharedMemory::addGameObject(fpsObject, GAME_LAYER_GUI_FOREGROUND);

	Game_Camera& mainCamera = Game_SharedMemory::w_mainCamera;
	int startTime = clock(), sleepTime = 16, frameCount = 0;

	// Rendering loop
	while (Game_SharedMemory::p_running)
	{
		SDL_RenderClear(mainRenderer);

		// Update camera
		if (mainCamera.m_movementDirection)
		{
			if (mainCamera.m_movementDirection & 0x1)
				mainCamera.m_position.y -= mainCamera.m_movementSpeed;
			else if (mainCamera.m_movementDirection & 0x2)
				mainCamera.m_position.y += mainCamera.m_movementSpeed;

			if (mainCamera.m_movementDirection & 0x4)
				mainCamera.m_position.x -= mainCamera.m_movementSpeed;
			else if (mainCamera.m_movementDirection & 0x8)
				mainCamera.m_position.x += mainCamera.m_movementSpeed;
		}

		// Draw all objects
		for (int i = GAME_LAYER_AMOUNT - 1; i >= 0; i--)
		{
			Game_RenderLayer* currentLayer = &Game_SharedMemory::r_renderLayers[i];
			Game_ObjectNode* currentObjectNode = currentLayer->objectList;
			while (currentObjectNode != NULL)
			{
				Game_Object* currentObject = currentObjectNode->object;

				// Frame-update object
				currentObject->frameUpdate();

				// Update texture
				if (currentObject->needsTextureUpdate())
				{
					// Destroy last rendered texture
					if (currentObject->m_lastRenderedTexture)
					{
						SDL_DestroyTexture(currentObject->m_lastRenderedTexture);
						currentObject->m_lastRenderedTexture = NULL;
					}

					// Create new surface
					// TODO: Use m_textureSize
					SDL_Surface* surface = SDL_CreateRGBSurface(0, currentObject->getSize().width, currentObject->getSize().height, 32, GAME_SURFACE_RMASK, GAME_SURFACE_GMASK, GAME_SURFACE_BMASK,
					GAME_SURFACE_AMASK);

					if (!surface)
						cout << "[WARN] Couldn't create surface for object rendering: " << SDL_GetError() << endl;
					else
					{
						SDL_Renderer* softwareRenderer = SDL_CreateSoftwareRenderer(surface);
						if (!softwareRenderer)
							cout << "[WARN] Couldn't create renderer for object rendering: " << SDL_GetError() << endl;
						else
						{
							// Prepare renderer
							SDL_SetRenderDrawColor(softwareRenderer, 0, 0, 0, 255);
							SDL_RenderClear(softwareRenderer);

							// Update object texture
							currentObject->textureUpdate(surface, softwareRenderer);
							currentObject->m_lastRenderedTexture = SDL_CreateTextureFromSurface(mainRenderer, surface);

							currentObject->m_needsTextureUpdate = false;
							SDL_DestroyRenderer(softwareRenderer);
						}

						SDL_FreeSurface(surface);
					}
				}

				// Draw object
				if (currentObject->m_lastRenderedTexture)
				{
					SDL_Rect destRect;
					switch (currentObject->getType())
					{
						case OBJECT_TYPE_NORMAL:
						case OBJECT_TYPE_GUI:
							destRect.x = currentObject->getCoords().x;
							destRect.y = currentObject->getCoords().y;
							destRect.w = currentObject->getSize().width;
							destRect.h = currentObject->getSize().height;
							break;
						case OBJECT_TYPE_WORLD:
							destRect.x = currentObject->getCoords().x - Game_SharedMemory::w_mainCamera.m_position.x;
							destRect.y = currentObject->getCoords().y - Game_SharedMemory::w_mainCamera.m_position.y;

							/* destRect.x *= Game_SharedMemory::w_zoomScale;
							 destRect.y *= Game_SharedMemory::w_zoomScale; */
							// TODO: Fix zooming system
							destRect.w = currentObject->getSize().width * Game_SharedMemory::w_zoomScale;
							destRect.h = currentObject->getSize().height * Game_SharedMemory::w_zoomScale;
							break;
					}

					if ((destRect.x + destRect.w) > 0 && (destRect.y + destRect.h) > 0)
						SDL_RenderCopy(mainRenderer, currentObject->m_lastRenderedTexture, NULL, &destRect);
				}

				currentObjectNode = currentObjectNode->nextNode;
			}
		}

		SDL_RenderPresent(mainRenderer);
		frameCount++;

		// Check if a second has passed
		int newTime = clock();
		if ((((float) newTime - (float) startTime) / CLOCKS_PER_SEC) >= 1.0f)
		{
			float clocksPerFrame = ((float) frameCount) / ((float) newTime - (float) startTime);
			float framesPerSecond = clocksPerFrame * CLOCKS_PER_SEC;

			// Display FPS if necessary
			if (Game_SharedMemory::p_useFPSCounter)
			{
				ostringstream stringStream;
				stringStream << "FPS: " << framesPerSecond;
				Game_SharedMemory::m_fpsObject->setText(stringStream.str());
			}

			// Update sleep time if necessary
			if (Game_SharedMemory::p_targetFPS > 0)
			{
				if (framesPerSecond < Game_SharedMemory::p_targetFPS - 5 || framesPerSecond > Game_SharedMemory::p_targetFPS + 5)
				{
					sleepTime *= framesPerSecond / Game_SharedMemory::p_targetFPS;
				}
			}

			startTime = newTime;
			frameCount = 0;
		}

		game_sleep(sleepTime);
	}

	cout << "[INFO] Stopped render thread" << endl;
}

// SDL

int game_initializeSDL(string windowTitle)
{
	if (Game_SharedMemory::s_SDLInitialized)
		return -1;

	// Initialize SDL
	int flags = SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER;
	if (SDL_Init(flags) == -1)
		return -2;

	// Initialize SDL image
	flags = IMG_INIT_PNG;
	if ((IMG_Init(flags) & flags) != flags)
		return -3;

	// Initialize SDL tty
	if (!TTF_WasInit() && TTF_Init() == -1)
		return -4;

	// Setup window
	flags = SDL_WINDOW_RESIZABLE;
	SDL_Window* window = SDL_CreateWindow(windowTitle.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1024, 576, flags);

	// Setup renderer
	flags = SDL_RENDERER_ACCELERATED;
	SDL_Renderer* windowRenderer = SDL_CreateRenderer(window, -1, flags);

	// Load GUI font
	// TODO: Create handler for assets folder
	Game_SharedMemory::m_guiFont = TTF_OpenFont("assets\\arial.ttf", 20);
	if (!Game_SharedMemory::m_guiFont)
		return -4;

	Game_SharedMemory::s_mainRenderer = windowRenderer;
	Game_SharedMemory::s_window = window;
	Game_SharedMemory::s_SDLInitialized = true;
	return 0;
}

void game_destroySDL()
{
	// Destroy renderer and window
	SDL_DestroyRenderer(Game_SharedMemory::s_mainRenderer);
	SDL_DestroyWindow(Game_SharedMemory::s_window);

	Game_SharedMemory::s_window = NULL;
	Game_SharedMemory::s_mainRenderer = NULL;
	Game_SharedMemory::s_SDLInitialized = false;

	// Destroy GUI font
	TTF_CloseFont(Game_SharedMemory::m_guiFont);

	// Quit SDL
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
}
