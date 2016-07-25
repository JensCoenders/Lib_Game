#include "game.h"

SDL_Texture* testObjectCT(Game_Object* object, Game_Rect bounds, SDL_Surface* surface, SDL_Renderer* softwareRenderer)
{
	// Test rendering
	SDL_SetRenderDrawColor(softwareRenderer, 255, 255, 255, 255);
	SDL_RenderClear(softwareRenderer);

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = bounds.width;
	rect.h = bounds.height;

	SDL_SetRenderDrawColor(softwareRenderer, 0, 255, 0, 255);
	SDL_RenderFillRect(softwareRenderer, &rect);

	cout << "Test object with ID " << object->getID() << " texture updated" << endl;
	return SDL_CreateTextureFromSurface(softwareRenderer, surface);
}

void testObjectFU(Game_Object* object)
{
	static short int movementDirection = 0;
	Game_EventObject* eventObject = (Game_EventObject*) object;

	while (eventObject->m_newKeyboardEvents > 0)
	{
		SDL_KeyboardEvent* event = eventObject->pollKeyboardEvent();
		bool keyDown = (event->type == SDL_KEYDOWN);

		switch (event->keysym.sym)
		{
			case SDLK_UP:
				if (keyDown)
					movementDirection |= 1;
				else
					movementDirection ^= 1;

				break;
			case SDLK_LEFT:
				if (keyDown)
					movementDirection |= 2;
				else
					movementDirection ^= 2;
				break;
			case SDLK_DOWN:
				if (keyDown)
					movementDirection |= 4;
				else
					movementDirection ^= 4;
				break;
			case SDLK_RIGHT:
				if (keyDown)
					movementDirection |= 8;
				else
					movementDirection ^= 8;
				break;
			case SDLK_SPACE:

				break;
		}
	}

	if ((movementDirection & 1) == 1)
		eventObject->m_worldCoords.y--;
	if ((movementDirection & 4) == 4)
		eventObject->m_worldCoords.y++;

	if ((movementDirection & 2) == 2)
		eventObject->m_worldCoords.x--;
	if ((movementDirection & 8) == 8)
		eventObject->m_worldCoords.x++;
}

void game_run()
{
	// Start render thread
	game_startRenderThread();

	// Create game objects
	Game_EventObject testObject(0, 0, 100, 100);
	testObject.m_frameUpdateFunc = testObjectFU;
	testObject.m_compileTextureFunc = testObjectCT;

	Game_SharedMemory::startRenderingObject(&testObject, 0);
	Game_SharedMemory::m_keyboardInputObject = &testObject;

	// Main loop
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
					game_processKeyboardEvent(&event);
					break;
					//case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN:
					//case SDL_MOUSEMOTION:
					game_processMouseEvent(&event);
					break;
				case SDL_WINDOWEVENT:
					game_processWindowEvent(&event);
					break;
			}
		}
	}

	// Join render thread
	game_joinRenderThread();

	// Clear shared memory
	delete[] Game_SharedMemory::r_layers;

	cout << "[INFO] Stopped main thread" << endl;
}
