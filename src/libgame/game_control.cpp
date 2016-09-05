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
	// Setup renderer
	SDL_Renderer* mainRenderer = game_shmGet(SHM_SDL_MAIN_RENDERER);
	SDL_SetRenderDrawColor(mainRenderer, 255, 255, 255, 0);

	// Setup FPS object
	if (!game_shmGet(SHM_MISC_FPS_OBJECT))
	{
		Game_Object fpsObject(-1, 0, 100, 50, true, MODULE_TEXT);
		fpsObject.setTextureUpdate(textObjectTextureUpdate);
		fpsObject.textModule->setText("FPS: 0");

		game_shmPut(SHM_MISC_FPS_OBJECT, &fpsObject);
		game_addGameObject(&fpsObject, GAME_LAYER_GUI_FOREGROUND);
	}

	Game_Camera& mainCamera = game_shmGet(SHM_WORLD_MAIN_CAMERA);
	int startTime = clock(), sleepTime = 16, frameCount = 0;

	// Rendering loop
	while (game_shmGet(SHM_GAME_IS_RUNNING))
	{
		SDL_RenderClear(mainRenderer);

		// Update camera
		if (mainCamera.movementDirection)
		{
			if (mainCamera.movementDirection & 0x1)
				mainCamera.position.y -= mainCamera.movementSpeed;
			else if (mainCamera.movementDirection & 0x2)
				mainCamera.position.y += mainCamera.movementSpeed;

			if (mainCamera.movementDirection & 0x4)
				mainCamera.position.x -= mainCamera.movementSpeed;
			else if (mainCamera.movementDirection & 0x8)
				mainCamera.position.x += mainCamera.movementSpeed;
		}

		// Draw all objects
		for (int i = GAME_LAYER_AMOUNT - 1; i >= 0; i--)
		{
			LinkedListNode<Game_Object>* currentObjectNode = game_shmGet(SHM_RENDER_LAYERS)[i].objectList;
			while (currentObjectNode)
			{
				Game_Object* currentObject = currentObjectNode->value;

				// Frame-update object
				currentObject->runFrameUpdate();

				// Update texture
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

				// Draw object
				if (currentObject->isVisible && currentObject->lastRenderedTexture)
				{
					SDL_Rect destRect;
					if (currentObject->isStatic)
					{
						destRect.x = currentObject->coords.x;
						destRect.y = currentObject->coords.y;
						destRect.w = currentObject->size.width;
						destRect.h = currentObject->size.height;
					}
					else
					{
						destRect.x = currentObject->coords.x - mainCamera.position.x;
						destRect.y = currentObject->coords.y - mainCamera.position.y;

						// TODO: Fix zooming system
						/* destRect.x *= Game_SharedMemory::w_zoomScale;
						 destRect.y *= Game_SharedMemory::w_zoomScale; */
						destRect.w = currentObject->size.width * game_shmGet(SHM_WORLD_ZOOM_SCALE);
						destRect.h = currentObject->size.height * game_shmGet(SHM_WORLD_ZOOM_SCALE);
					}

					if (destRect.w < 0)
						destRect.w += (mainCamera.size.width + 1);
					if (destRect.h < 0)
						destRect.h += (mainCamera.size.height + 1);

					if (destRect.x < 0)
						destRect.x += (mainCamera.size.width - destRect.w + 1);
					if (destRect.y < 0)
						destRect.y += (mainCamera.size.height - destRect.h + 1);

					if ((destRect.x + destRect.w) > 0 && (destRect.y + destRect.h) > 0)
						SDL_RenderCopy(mainRenderer, currentObject->lastRenderedTexture, NULL, &destRect);
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
			if (game_shmGet(SHM_MISC_FPS_OBJECT)->isModuleEnabled(MODULE_TEXT) && game_shmGet(SHM_GAME_USE_FPS_COUNTER))
			{
				ostringstream stringStream;
				stringStream << "FPS: " << framesPerSecond;
				game_shmGet(SHM_MISC_FPS_OBJECT)->textModule->setText(stringStream.str());
			}

			// Update sleep time if necessary
			int targetFps = game_shmGet(SHM_GAME_TARGET_FPS);
			if (targetFps > 0)
			{
				if (framesPerSecond < targetFps - 2 || framesPerSecond > targetFps + 2)
					sleepTime *= framesPerSecond / targetFps;
			}

			startTime = newTime;
			frameCount = 0;
		}

		game_sleep(sleepTime);
	}

	cout << "[INFO] Stopped render thread" << endl;
}

void game_startRenderThread()
{
	game_shmPut(SHM_GAME_IS_RUNNING, true);

	if ((g_renderThreadID = game_startThread(game_renderThread)))
		cout << "[ERR] Couldn't start render thread! Error code: " << g_renderThreadID << endl;
	else
		cout << "[INFO] Started render thread with ID " << g_renderThreadID << endl;
}

void game_stopRenderThread(bool join)
{
	game_shmPut(SHM_GAME_IS_RUNNING, false);

	if (join)
		game_joinThread(g_renderThreadID);
}

int game_initialize(string windowTitle, Game_Rect windowSize, Game_Point windowStartPos)
{
	if (game_shmGet(SHM_SDL_INITIALIZED))
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
	SDL_Window* window = SDL_CreateWindow(windowTitle.c_str(), windowStartPos.x, windowStartPos.y, windowSize.width,
	        windowSize.height, flags);

	// Setup renderer
	flags = SDL_RENDERER_ACCELERATED;
	SDL_Renderer* mainRenderer = SDL_CreateRenderer(window, -1, flags);

	// Load GUI font
	TTF_Font* newFont = TTF_OpenFont(game_getAssetPath("FantasqueSansMono.ttf", "fonts").c_str(), 25);
	if (!newFont)
		return -4;

	game_shmGet(SHM_WORLD_MAIN_CAMERA).size = windowSize;
	game_shmPut(SHM_SDL_WINDOW, window);
	game_shmPut(SHM_SDL_MAIN_RENDERER, mainRenderer);
	game_shmPut(SHM_MISC_GUI_FONT, newFont);
	game_shmPut(SHM_SDL_INITIALIZED, true);
	return 0;
}

void game_cleanup()
{
	// Destroy renderer and window
	SDL_DestroyRenderer(game_shmGet(SHM_SDL_MAIN_RENDERER));
	SDL_DestroyWindow(game_shmGet(SHM_SDL_WINDOW));

	game_shmPut(SHM_SDL_WINDOW, NULL);
	game_shmPut(SHM_SDL_MAIN_RENDERER, NULL);
	game_shmPut(SHM_SDL_INITIALIZED, false);

	delete[] game_shmGet(SHM_RENDER_LAYERS);

	// Destroy GUI font
	TTF_CloseFont(game_shmGet(SHM_MISC_GUI_FONT));

	// Quit SDL
	SDL_Quit();
	IMG_Quit();
	TTF_Quit();
}
