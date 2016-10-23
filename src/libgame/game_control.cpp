#include <iostream>
#include <time.h>
#include <sstream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "game_control.h"
#include "game_defs.h"
#include "game_event.h"
#include "game_property.h"
#include "game_thread.h"
#include "game_tools.h"

using namespace std;

int g_renderThreadID = -1;

void game_runMainLoop()
{
	// Start render thread
	game_startRenderThread();

	GAME_DEBUG_CHECK
		cout << "[DEBUG] Started GUI thread" << endl;

	while (gameVar_isRunning)
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

	GAME_DEBUG_CHECK
		cout << "[DEBUG] Stopped GUI thread" << endl;
}

void game_renderThread()
{
	// Setup renderer
	SDL_SetRenderDrawColor(gameVar_mainRenderer, 255, 255, 255, 0);

	// Setup FPS object
	if (!gameVar_fpsObject)
	{
		GAME_DEBUG_CHECK
			cout << "[DEBUG] Setting up FPS object... ";

		Game_Object* fpsObject = new Game_Object(5, 0, 0, 0, true, game_combineModules(MODULE_TEXT, MODULE_PROPERTY));
		fpsObject->setTextureUpdate(textObjectTU);
		fpsObject->textModule->setText("FPS: 0");
		fpsObject->textModule->setTextColor( {255, 255, 0, 255});
		fpsObject->propertyModule->setProperty("LIBGAME_CREATED", true);

		gameVar_fpsObject = fpsObject;
		game_addGameObject(fpsObject, GAME_LAYER_GUI_FOREGROUND);

		GAME_DEBUG_CHECK
			cout << "[OK]" << endl;
	}

	// Rendering loop
	int startTime = clock(), sleepTime = 15, frameCount = 0;
	while (gameVar_isRunning)
	{
		SDL_RenderClear(gameVar_mainRenderer);

		// Update all objects
		for (int i = 0; i < GAME_LAYER_AMOUNT; i++)
		{
			LinkedListNode<Game_Object>* currentNode = gameVar_renderLayers[i].objectList;
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
					SDL_Surface* surface = currentObject->runTextureUpdate(*equipment);
					if (surface)
					{
						currentObject->lastRenderedTexture = SDL_CreateTextureFromSurface(gameVar_mainRenderer, surface);
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
		if (gameVar_keyboardMovesCamera)
		{
			unsigned char movementDirection = gameVar_mainCamera.movementDirection;
			int movementSpeed = gameVar_mainCamera.movementSpeed;
			Game_Point* position = &gameVar_mainCamera.position;

			if (movementDirection & 0x1)
				position->y -= movementSpeed;
			else if (movementDirection & 0x2)
				position->y += movementSpeed;

			if (movementDirection & 0x4)
				position->x -= movementSpeed;
			else if (movementDirection & 0x8)
				position->x += movementSpeed;
		}
		else if (gameVar_mainCamera.centeredObject)
		{
			gameVar_mainCamera.position.x = -(gameVar_mainCamera.size.width - gameVar_mainCamera.centeredObject->size.width) / 2 +
					gameVar_mainCamera.centeredObject->position.x;
			gameVar_mainCamera.position.y = -(gameVar_mainCamera.size.height - gameVar_mainCamera.centeredObject->size.height) / 2 +
					gameVar_mainCamera.centeredObject->position.y;
		}

		// Draw all objects
		for (int i = GAME_LAYER_AMOUNT - 1; i >= 0; i--)
		{
			LinkedListNode<Game_Object>* currentNode = gameVar_renderLayers[i].objectList;
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

					if (currentObject->isStatic || game_isInside( {0, 0}, gameVar_mainCamera.size, renderPos, renderSize, false))
					{
						SDL_RenderCopyEx(gameVar_mainRenderer, currentObject->lastRenderedTexture, NULL, &renderRect,
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
			if (!gameVar_keyboardMovesCamera && gameVar_mainCamera.centeredObject)
			{
				indicRectCamPos.x = (gameVar_mainCamera.size.width - gameVar_mainCamera.centeredObject->size.width) / 2 -
						gameVar_mainCamera.centeredObject->position.x - 5;
				indicRectCamPos.y = (gameVar_mainCamera.size.height - gameVar_mainCamera.centeredObject->size.height) / 2 -
						gameVar_mainCamera.centeredObject->position.y - 5;
			}
			else
			{
				indicRectCamPos.x = -gameVar_mainCamera.position.x - 5;
				indicRectCamPos.y = -gameVar_mainCamera.position.y - 5;
			}

			if (indicRectCamPos.x - 5 <= 0)
				indicRectCamPos.x = 5;
			if (indicRectCamPos.y - 5 <= 0)
				indicRectCamPos.y = 5;

			indicRectCamPos.w = 10;
			indicRectCamPos.h = 10;

			SDL_SetRenderDrawColor(gameVar_mainRenderer, 255, 0, 0, 0);
			SDL_RenderFillRect(gameVar_mainRenderer, &indicRectCamPos);

			SDL_Surface* textSurface = TTF_RenderText_Blended(gameVar_guiFont, "(0, 0)", {0, 0, 255, 0});
			SDL_Texture* textTexture = SDL_CreateTextureFromSurface(gameVar_mainRenderer, textSurface);

			SDL_Rect textRect;
			textRect.x = (gameVar_mainCamera.position.x + 40 < 0 ? gameVar_mainCamera.position.x * -1 : 43) - 40;
			textRect.y = (gameVar_mainCamera.position.y + 35 < 0 ? gameVar_mainCamera.position.y * -1 : 38) - 35;
			textRect.w = textSurface->w;
			textRect.h = textSurface->h;

			SDL_RenderCopy(gameVar_mainRenderer, textTexture, NULL, &textRect);
			SDL_FreeSurface(textSurface);
			SDL_DestroyTexture(textTexture);

			SDL_SetRenderDrawColor(gameVar_mainRenderer, 255, 255, 255, 0);
		}

		SDL_RenderPresent(gameVar_mainRenderer);
		frameCount++;

		// Check if a second has passed
		int newTime = clock();
		if (((float) (newTime - startTime) / CLOCKS_PER_SEC) >= 1.0f)
		{
			float fps = ((float) frameCount) / ((float) (newTime - startTime)) * CLOCKS_PER_SEC;

			// Display FPS if necessary
			if (gameVar_useFpsCounter && gameVar_fpsObject->isModuleEnabled(MODULE_TEXT))
			{
				ostringstream stringStream;
				stringStream << "FPS: " << fps;
				gameVar_fpsObject->textModule->setText(stringStream.str());

				GAME_DEBUG_CHECK
					cout << "[DEBUG] " << stringStream.str() << endl;
			}

			// Update sleep time if necessary
			int targetFps = gameVar_targetFps;
			if (targetFps > 0 && (fps < targetFps - 2 || fps > targetFps + 2))
				sleepTime *= fps / targetFps;

			startTime = newTime;
			frameCount = 0;
		}

		game_sleep(sleepTime);
	}

	// Delete FPS object if it wasn't manually created
	if (gameVar_fpsObject && gameVar_fpsObject->isModuleEnabled(MODULE_PROPERTY) &&
			gameVar_fpsObject->propertyModule->getBoolProperty("LIBGAME_CREATED", false))
	{
		delete gameVar_fpsObject;

		GAME_DEBUG_CHECK
			cout << "[DEBUG] Deleted self-constructed FPS object" << endl;
	}

	GAME_DEBUG_CHECK
		cout << "[DEBUG] Stopped render thread (thread with ID " << g_renderThreadID << ')' << endl;

}

bool game_startRenderThread()
{
	gameVar_isRunning = true;

	if ((g_renderThreadID = game_startThread(game_renderThread)))
	{
		cout << "[ERROR] Couldn't start render thread! Error code: " << g_renderThreadID << endl;
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
	gameVar_isRunning = false;

	if (join)
	{
		GAME_DEBUG_CHECK
			cout << "[DEBUG] Joining render thread..." << endl;

		game_joinThread(g_renderThreadID);
	}
}

int game_initialize(string windowTitle, Game_Rect windowSize, Game_Point windowStartPos)
{
	if (gameVar_SDLInitialized)
		return -1;

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
	SDL_Window* mainWindow = SDL_CreateWindow(windowTitle.c_str(), windowStartPos.x, windowStartPos.y,
		windowSize.width, windowSize.height, flags);

	// Setup renderer
	flags = SDL_RENDERER_ACCELERATED;
	SDL_Renderer* mainRenderer = SDL_CreateRenderer(mainWindow, -1, flags);

	// Load GUI font
	TTF_Font* guiFont = TTF_OpenFont(game_getAssetPath("FantasqueSansMono.ttf", "fonts").c_str(), 25);
	if (!guiFont)
		return -4;

	// Set properties
	gameVar_SDLInitialized = true;
	gameVar_mainWindow = mainWindow;
	gameVar_mainRenderer = mainRenderer;
	gameVar_mainCamera.size = windowSize;
	gameVar_guiFont = guiFont;

	return 0;
}

void game_cleanup()
{
	GAME_DEBUG_CHECK
		cout << "[DEBUG] Cleaning up..." << endl;

	// Free assets
	game_freeAssets();

	// Free object rendering list
	delete[] gameVar_renderLayers;

	// Destroy GUI font
	TTF_CloseFont(gameVar_guiFont);

	// Destroy renderer and window
	SDL_DestroyRenderer(gameVar_mainRenderer);
	SDL_DestroyWindow(gameVar_mainWindow);

	gameVar_SDLInitialized = false;
	gameVar_mainRenderer = NULL;
	gameVar_mainWindow = NULL;

	// Quit SDL
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
}
