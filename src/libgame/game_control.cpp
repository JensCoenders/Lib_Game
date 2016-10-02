#include <iostream>
#include <time.h>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "game_control.h"
#include "game_defs.h"
#include "game_event.h"
#include "game_shm.h"
#include "game_thread.h"
#include "game_tools.h"

using namespace std;

int g_renderThreadID = -1;

void game_runMainLoop()
{
	// Start render thread
	game_startRenderThread();

	cout << "[INFO] Started main thread" << endl;
	while (game_shmGet(SHM_GAME_IS_RUNNING))
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
				case SDL_MOUSEBUTTONUP:
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
	game_stopRenderThread(true);
}

void game_renderThread()
{
	cout << "[INFO] Started render thread" << endl;

	// Setup renderer
	SDL_Renderer* mainRenderer = game_shmGet(SHM_SDL_MAIN_RENDERER);
	SDL_SetRenderDrawColor(mainRenderer, 255, 255, 255, 0);

	// Setup FPS object
	if (!game_shmGet(SHM_MISC_FPS_OBJECT))
	{
		GAME_DEBUG_CHECK
			cout << "[DEBUG] Setting up FPS object... ";

		Game_Object* fpsObject = new Game_Object(5, 0, 0, 0, true, game_combineModules(MODULE_TEXT, MODULE_PROPERTY));
		fpsObject->setTextureUpdate(textObjectTextureUpdate);
		fpsObject->textModule->setText("FPS: 0");
		fpsObject->textModule->setTextColor({255, 255, 0, 255});
		fpsObject->propertyModule->setProperty("LIBGAME_CREATED", true);

		game_shmPut(SHM_MISC_FPS_OBJECT, fpsObject);
		game_addGameObject(fpsObject, GAME_LAYER_GUI_FOREGROUND);

		GAME_DEBUG_CHECK
			cout << "[OK]" << endl;
	}

	// Rendering loop
	Game_Camera* mainCamera = &game_shmGet(SHM_WORLD_MAIN_CAMERA);
	int startTime = clock(), sleepTime = 15, frameCount = 0;

	while (game_shmGet(SHM_GAME_IS_RUNNING))
	{
		SDL_RenderClear(mainRenderer);

		// Update all objects
		for (int i = 0; i < GAME_LAYER_AMOUNT; i++)
		{
			LinkedListNode<Game_Object>* currentNode = game_shmGet(SHM_RENDER_LAYERS)[i].objectList;
			while (currentNode)
			{
				Game_Object* currentObject = currentNode->value;

				// Frame-update object
				currentObject->runFrameUpdate();

				// Texture-update object
				if (currentObject->needsTextureUpdate())
				{
					// Destroy last rendered texture
					if (currentObject->lastRenderedTexture)
					{
						SDL_DestroyTexture(currentObject->lastRenderedTexture);
						currentObject->lastRenderedTexture = NULL;
					}

					// Create render equipment
					Game_RenderEquipment* equipment = NULL;
					if (currentObject->size.width > 0 && currentObject->size.height > 0)
					{
						equipment = game_createRenderEquipment(currentObject->size.width, currentObject->size.height);

						// Prepare renderer
						SDL_SetRenderDrawColor(equipment->softwareRenderer, 0, 0, 0, 255);
						SDL_RenderClear(equipment->softwareRenderer);
					}

					// Update object texture
					SDL_Surface* surface = currentObject->runTextureUpdate(equipment);
					if (surface)
					{
						currentObject->lastRenderedTexture = SDL_CreateTextureFromSurface(mainRenderer, surface);
						if (!currentObject->lastRenderedTexture)
							cout << "[ERR] Couldn't create texture from surface: " << SDL_GetError() << endl;
						else
							currentObject->satisfyTextureUpdate();

						if (equipment && surface != equipment->surface)
							SDL_FreeSurface(surface);
					}

					if (equipment)
						delete equipment;
				}

				currentNode = currentNode->nextNode;
			}
		}

		// Update camera position
		if (game_shmGet(SHM_WORLD_KEYBOARD_MOVES_CAMERA))
		{
			if (mainCamera->movementDirection & 0x1)
				mainCamera->position.y -= mainCamera->movementSpeed;
			else if (mainCamera->movementDirection & 0x2)
				mainCamera->position.y += mainCamera->movementSpeed;

			if (mainCamera->movementDirection & 0x4)
				mainCamera->position.x -= mainCamera->movementSpeed;
			else if (mainCamera->movementDirection & 0x8)
				mainCamera->position.x += mainCamera->movementSpeed;
		}
		else if (mainCamera->centeredObject)
		{
			// Frame-update centered object
			mainCamera->centeredObject->runFrameUpdate();

			mainCamera->position.x = -(mainCamera->size.width - mainCamera->centeredObject->size.width) / 2 +
					mainCamera->centeredObject->position.x;
			mainCamera->position.y = -(mainCamera->size.height - mainCamera->centeredObject->size.height) / 2 +
					mainCamera->centeredObject->position.y;
		}

		// Draw all objects
		for (int i = GAME_LAYER_AMOUNT - 1; i >= 0; i--)
		{
			LinkedListNode<Game_Object>* currentNode = game_shmGet(SHM_RENDER_LAYERS)[i].objectList;
			while (currentNode)
			{
				Game_Object* currentObject = currentNode->value;
				if (currentObject->isVisible && currentObject->lastRenderedTexture)
				{
					// Get render position and size
					Game_Point renderPos = game_getObjectRenderPos(*currentObject);
					Game_Rect renderSize = game_getObjectRenderSize(*currentObject);

					SDL_Rect renderRect;
					renderRect.x = renderPos.x;
					renderRect.y = renderPos.y;
					renderRect.w = renderSize.width;
					renderRect.h = renderSize.height;

					if (currentObject->isStatic || game_isInside({0, 0}, mainCamera->size, renderPos, renderSize, false))
					{
						SDL_RenderCopyEx(mainRenderer, currentObject->lastRenderedTexture, NULL, &renderRect,
						        currentObject->rotation, NULL, SDL_FLIP_NONE);

						currentObject->isOutsideCameraBounds = false;
					}
					else if (!currentObject->isOutsideCameraBounds)
						currentObject->isOutsideCameraBounds = true;
				}

				currentNode = currentNode->nextNode;
			}
		}

		// Draw the camera
		GAME_DEBUG_CHECK
		{
			SDL_Rect indicRectCamPos;
			if (!game_shmGet(SHM_WORLD_KEYBOARD_MOVES_CAMERA) && mainCamera->centeredObject)
			{
				indicRectCamPos.x = (mainCamera->size.width - mainCamera->centeredObject->size.width) / 2
						- mainCamera->centeredObject->position.x - 5;
				indicRectCamPos.y = (mainCamera->size.height - mainCamera->centeredObject->size.height) / 2
						- mainCamera->centeredObject->position.y - 5;
			}
			else
			{
				indicRectCamPos.x = -mainCamera->position.x - 5;
				indicRectCamPos.y = -mainCamera->position.y - 5;
			}

			if (indicRectCamPos.x - 5 <= 0)
				indicRectCamPos.x = 5;
			if (indicRectCamPos.y - 5 <= 0)
				indicRectCamPos.y = 5;

			indicRectCamPos.w = 10;
			indicRectCamPos.h = 10;

			SDL_SetRenderDrawColor(mainRenderer, 255, 0, 0, 0);
			SDL_RenderFillRect(mainRenderer, &indicRectCamPos);

			SDL_Surface* textSurface = TTF_RenderText_Blended(game_shmGet(SHM_MISC_GUI_FONT), "(0, 0)", {0, 0, 255, 0});
			SDL_Texture* textTexture = SDL_CreateTextureFromSurface(mainRenderer, textSurface);

			SDL_Rect textRect;
			textRect.x = (mainCamera->position.x + 40 < 0? mainCamera->position.x * -1 : 43) - 40;
			textRect.y = (mainCamera->position.y + 35 < 0? mainCamera->position.y * -1 : 38) - 35;
			textRect.w = textSurface->w;
			textRect.h = textSurface->h;

			SDL_RenderCopy(mainRenderer, textTexture, NULL, &textRect);
			SDL_FreeSurface(textSurface);
			SDL_DestroyTexture(textTexture);

			SDL_SetRenderDrawColor(mainRenderer, 255, 255, 255, 0);
		}

		SDL_RenderPresent(mainRenderer);
		frameCount++;

		// Check if a second has passed
		int newTime = clock();
		if (((float) (newTime - startTime) / CLOCKS_PER_SEC) >= 1.0f)
		{
			float fps = ((float) frameCount) / ((float) (newTime - startTime)) * CLOCKS_PER_SEC;

			// Display FPS if necessary
			if (game_shmGet(SHM_GAME_USE_FPS_COUNTER) && game_shmGet(SHM_MISC_FPS_OBJECT)->isModuleEnabled(MODULE_TEXT))
			{
				ostringstream stringStream;
				stringStream << "FPS: " << fps;
				game_shmGet(SHM_MISC_FPS_OBJECT)->textModule->setText(stringStream.str());

				GAME_DEBUG_CHECK
					cout << "[DEBUG] " << stringStream.str() << endl;
			}

			// Update sleep time if necessary
			int targetFps = game_shmGet(SHM_GAME_TARGET_FPS);
			if (targetFps > 0 && (fps < targetFps - 2 || fps > targetFps + 2))
				sleepTime *= fps / targetFps;

			startTime = newTime;
			frameCount = 0;
		}

		game_sleep(sleepTime);
	}

	// Delete FPS object if it wasn't manually created
	if (game_shmGet(SHM_MISC_FPS_OBJECT) && game_shmGet(SHM_MISC_FPS_OBJECT)->isModuleEnabled(MODULE_PROPERTY)
	        && game_shmGet(SHM_MISC_FPS_OBJECT)->propertyModule->getBoolProperty("LIBGAME_CREATED", false))
	{
		delete game_shmGet(SHM_MISC_FPS_OBJECT);

		GAME_DEBUG_CHECK
			cout << "[DEBUG] Deleted self-constructed FPS object" << endl;
	}

	GAME_DEBUG_CHECK
		cout << "[DEBUG] Stopped render thread (thread with ID " << g_renderThreadID << ')' << endl;
	else
		cout << "[INFO] Stopped render thread" << endl;
}

bool game_startRenderThread()
{
	game_shmPut(SHM_GAME_IS_RUNNING, true);

	if ((g_renderThreadID = game_startThread(game_renderThread)))
	{
		cout << "[ERR] Couldn't start render thread! Error code: " << g_renderThreadID << endl;
		return false;
	}
	else
	{
		GAME_DEBUG_CHECK
			cout << "[DEBUG] Started render thread with ID " << g_renderThreadID << endl;
	}

	return true;
}

void game_stopRenderThread(bool join)
{
	game_shmPut(SHM_GAME_IS_RUNNING, false);

	if (join)
	{
		GAME_DEBUG_CHECK
			cout << "[DEBUG] Joining render thread..." << endl;

		game_joinThread(g_renderThreadID);
	}
}

int game_initialize(string windowTitle, Game_Rect windowSize, Game_Point windowStartPos)
{
	if (game_shmGet(SHM_SDL_INITIALIZED))
		return -1;

#ifdef GAME_DEBUG
	game_shmPut(SHM_GAME_DEBUG_MODE, true);
#endif

	GAME_DEBUG_CHECK
		cout << "[DEBUG] Initializing SDL... ";

	// Initialize SDL
	int flags = SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_EVENTS | SDL_INIT_TIMER;
	if (SDL_Init(flags) == -1)
		return -2;

	// Initialize SDL image
	flags = IMG_INIT_PNG;
	if ((IMG_Init(flags) & flags) != flags)
		return -3;

	// Initialize SDL ttf
	if (!TTF_WasInit() && TTF_Init() == -1)
		return -4;

	// Setup window
	flags = SDL_WINDOW_RESIZABLE;
	SDL_Window* window = SDL_CreateWindow(windowTitle.c_str(), windowStartPos.x, windowStartPos.y, windowSize.width,
	        windowSize.height, flags);

	// Setup renderer
	flags = SDL_RENDERER_ACCELERATED;
	SDL_Renderer* mainRenderer = SDL_CreateRenderer(window, -1, flags);

	// Load GUI font
	TTF_Font* newFont = TTF_OpenFont(game_getAssetPath("FantasqueSansMono.ttf", "fonts").c_str(), 25);
	if (!newFont)
		return -4;

	// Set SHM variables
	game_shmPut(SHM_SDL_INITIALIZED, true);
	game_shmPut(SHM_SDL_WINDOW, window);
	game_shmPut(SHM_SDL_MAIN_RENDERER, mainRenderer);
	game_shmGet(SHM_WORLD_MAIN_CAMERA).size = windowSize;
	game_shmPut(SHM_MISC_GUI_FONT, newFont);

	GAME_DEBUG_CHECK
		cout << "[OK]" << endl;
	return 0;
}

void game_cleanup()
{
	GAME_DEBUG_CHECK
		cout << "[DEBUG] Cleaning up..." << endl;

	// Free assets
	game_freeAssets();

	// Free object rendering list
	delete[] game_shmGet(SHM_RENDER_LAYERS);

	// Destroy GUI font
	TTF_CloseFont(game_shmGet(SHM_MISC_GUI_FONT));

	// Destroy renderer and window
	SDL_DestroyRenderer(game_shmGet(SHM_SDL_MAIN_RENDERER));
	SDL_DestroyWindow(game_shmGet(SHM_SDL_WINDOW));

	game_shmPut(SHM_SDL_WINDOW, NULL);
	game_shmPut(SHM_SDL_MAIN_RENDERER, NULL);
	game_shmPut(SHM_SDL_INITIALIZED, false);

	// Quit SDL
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
}
