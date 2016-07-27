#include <time.h>
#include <SDL_image.h>
#include "control.h"
#include "thread.h"

using namespace std;

/* Shared Memory */

bool Game_SharedMemory::p_running = false;
int Game_SharedMemory::p_targetFPS = 60;
bool Game_SharedMemory::p_useFPSCounter = true;
float Game_SharedMemory::p_zoomScale = 1.0f;

Game_Point Game_SharedMemory::r_cameraPos = {0, 0};
Game_Rect Game_SharedMemory::r_cameraBounds = {0, 0};
Game_Layer* Game_SharedMemory::r_layers = new Game_Layer[GAME_LAYER_AMOUNT];
int Game_SharedMemory::r_renderThreadID = -1;

bool Game_SharedMemory::s_SDLInitialized = false;
SDL_Renderer* Game_SharedMemory::s_mainRenderer = NULL;
SDL_Window* Game_SharedMemory::s_window = NULL;

Game_Object* Game_SharedMemory::m_keyboardInputObject = NULL;

/* Extra control functions */

// Rendering
void game_renderThread();

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
	delete objectList;   // Deleting the first object node will destroy the whole linked list
}

/* Shared Memory */
bool Game_SharedMemory::startRenderingObject(Game_Object* object, unsigned int layerID)
{
	if (layerID >= GAME_LAYER_AMOUNT)
	{
		cout << "[WARN] Invalid layer ID provided: " << layerID << endl;
		return false;
	}

	Game_Layer* layer = &r_layers[layerID];
	Game_ObjectNode* newObjectNode = new Game_ObjectNode();
	newObjectNode->object = object;
	newObjectNode->nextNode = layer->objectList;

	layer->objectList = newObjectNode;
	layer->objectCount++;

	return true;
}

bool Game_SharedMemory::stopRenderingObject(Game_Object* object)
{
	for (int i = 0; i < GAME_LAYER_AMOUNT; i++)
	{
		Game_Layer* currentLayer = &r_layers[i];
		Game_ObjectNode* currentObjectNode = currentLayer->objectList;
		while (currentObjectNode != NULL)
		{
			if (currentObjectNode->object->getID() == object->getID())
			{
				if (currentObjectNode->prevNode != NULL)
				{
					currentObjectNode->prevNode->nextNode = currentObjectNode->nextNode;
				}

				if (currentObjectNode->nextNode != NULL)
				{
					currentObjectNode->nextNode->prevNode = currentObjectNode->prevNode;
				}

				currentObjectNode->prevNode = NULL;
				currentObjectNode->nextNode = NULL;
				delete currentObjectNode;
				currentLayer->objectCount--;

				return true;
			}
		}
	}

	return false;
}

/* Control functions */

// Rendering
void game_startRenderThread()
{
	Game_SharedMemory::p_running = true;

	// Start thread
	Game_SharedMemory::r_renderThreadID = game_startThread(game_renderThread);
	if (Game_SharedMemory::r_renderThreadID < 0)
	{
		cout << "[ERR] Couldn't start render thread! Error code: " << Game_SharedMemory::r_renderThreadID << endl;
	}
	else
	{
		cout << "[INFO] Started render thread with ID " << Game_SharedMemory::r_renderThreadID << endl;
	}
}

void game_joinRenderThread()
{
	game_joinThread(Game_SharedMemory::r_renderThreadID);
}

void game_renderThread()
{
	// Setup renderer
	SDL_Renderer* mainRenderer = Game_SharedMemory::s_mainRenderer;
	SDL_SetRenderDrawColor(mainRenderer, 0, 0, 0, 0);

	// Rendering loop
	int frameCount = 0, sleepTime = 10, startTime = clock();
	while (Game_SharedMemory::p_running)
	{
		SDL_RenderClear(mainRenderer);

		// Draw all objects
		for (int i = 0; i < GAME_LAYER_AMOUNT; i++)
		{
			Game_Layer* currentLayer = &Game_SharedMemory::r_layers[i];
			Game_ObjectNode* currentObjectNode = currentLayer->objectList;
			while (currentObjectNode != NULL)
			{
				Game_Object* currentObject = currentObjectNode->object;
				Game_Rect realBounds;
				realBounds.width = currentObject->m_worldSize.width * Game_SharedMemory::p_zoomScale;
				realBounds.height = currentObject->m_worldSize.height * Game_SharedMemory::p_zoomScale;

				// Frame-update object
				currentObject->frameUpdate();

				// Re-render object if necessary
				if (currentObject->needsTextureUpdate())
				{
					if (currentObject->m_lastRenderedTexture != NULL)
					{
						SDL_DestroyTexture(currentObject->m_lastRenderedTexture);
						currentObject->m_lastRenderedTexture = NULL;
					}

					SDL_Surface* surface = SDL_CreateRGBSurface(0, realBounds.width, realBounds.height, 32, 0, 0, 0, 0);
					if (surface == NULL)
					{
						cout << "[WARN] Couldn't create surface for object rendering: " << SDL_GetError() << endl;
					}
					else
					{
						SDL_Renderer* softwareRenderer = SDL_CreateSoftwareRenderer(surface);
						if (softwareRenderer == NULL)
						{
							cout << "[WARN] Couldn't create renderer for object rendering: " << SDL_GetError() << endl;
						}
						else
						{
							currentObject->textureUpdate(realBounds, softwareRenderer);
							currentObject->m_lastRenderedTexture = SDL_CreateTextureFromSurface(mainRenderer, surface);
							if (!currentObject->m_lastRenderedTexture)
							{
								cout << "[WARN] Couldn't render object texture: " << SDL_GetError() << endl;
							}
							else
							{
								currentObject->m_needsTextureUpdate = false;
							}

							SDL_DestroyRenderer(softwareRenderer);
						}

						SDL_FreeSurface(surface);
					}
				}

				if (currentObject->m_lastRenderedTexture)
				{
					// Draw object
					// TODO: Implement camera bounds
					SDL_Rect targetRect;
					targetRect.x = currentObject->m_worldCoords.x;
					targetRect.y = currentObject->m_worldCoords.y;
					targetRect.w = realBounds.width;
					targetRect.h = realBounds.height;

					SDL_RenderCopy(mainRenderer, currentObject->m_lastRenderedTexture, NULL, &targetRect);
				}

				currentObjectNode = currentObjectNode->nextNode;
			}
		}

		SDL_RenderPresent(mainRenderer);
		frameCount++;

		// Calculate FPS
		int newTime = clock();
		if (((float) (newTime - startTime) / CLOCKS_PER_SEC) >= 1.0f)
		{
			if (Game_SharedMemory::p_useFPSCounter)
			{
				cout << "[INFO] FPS: " << frameCount << endl;
			}

			// Reach target frame rate
			if (Game_SharedMemory::p_targetFPS > 0)
			{
				if (frameCount > (Game_SharedMemory::p_targetFPS + 5))
				{
					sleepTime += 1;
				}
				else if (frameCount < (Game_SharedMemory::p_targetFPS - 5) && sleepTime > 0)
				{
					sleepTime -= 1;
				}
			}

			startTime = newTime;
			frameCount = 0;
		}

		if (Game_SharedMemory::p_targetFPS > 0)
		{
			game_sleep(sleepTime);
		}
	}

	cout << "[INFO] Stopped render thread" << endl;
}

// SDL

Game_Result game_initializeSDL(string windowTitle)
{
	Game_Result result = {GAME_SUCCESS, ""};

	if (Game_SharedMemory::s_SDLInitialized)
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

	Game_SharedMemory::s_mainRenderer = windowRenderer;
	Game_SharedMemory::s_window = window;
	Game_SharedMemory::s_SDLInitialized = true;
	return result;
}

void game_destroySDL()
{
	// Destroy renderer and window
	SDL_DestroyRenderer(Game_SharedMemory::s_mainRenderer);
	SDL_DestroyWindow(Game_SharedMemory::s_window);

	// Quit SDL
	SDL_Quit();
	IMG_Quit();

	Game_SharedMemory::s_window = NULL;
	Game_SharedMemory::s_mainRenderer = NULL;
	Game_SharedMemory::s_SDLInitialized = false;
}

// Events

void game_processKeyboardEvent(SDL_Event* event)
{
	switch (event->key.keysym.sym)
	{
		case SDLK_F11:
			if (event->type == SDL_KEYDOWN)
			{
				static int lastFlag = 0;
				lastFlag = (lastFlag == 0 ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

				SDL_SetWindowFullscreen(Game_SharedMemory::s_window, lastFlag);
			}
			break;
		default:
			if (Game_SharedMemory::m_keyboardInputObject)
			{
				Game_SharedMemory::m_keyboardInputObject->callEventFunction(TYPE_KEY_TYPED, event);
			}
			break;
	}
}

void game_processMouseEvent(SDL_Event* event)
{

}

void game_processWindowEvent(SDL_Event* event)
{
	switch (event->window.event)
	{
		case SDL_WINDOWEVENT_RESIZED:
			break;
		case SDL_WINDOWEVENT_CLOSE:
			Game_SharedMemory::p_running = false;
			break;
	}
}
