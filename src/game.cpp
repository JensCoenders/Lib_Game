#include "game.h"

void testObjectFU(Game_Object* object)
{
	int direction = object->getProperty("direction", 0);
	int speed = object->getProperty("speed", 1);

	if ((direction & 1) == 1)
		object->m_worldCoords.y -= speed;
	if ((direction & 4) == 4)
		object->m_worldCoords.y += speed;

	if ((direction & 2) == 2)
		object->m_worldCoords.x -= speed;
	if ((direction & 8) == 8)
		object->m_worldCoords.x += speed;

	if (object->getProperty("flashing", 0))
	{
		static bool lastColorRed = false;
		if (lastColorRed)
			object->setProperty("color", 0xFF00);
		else
			object->setProperty("color", 0xFF);

		lastColorRed = (lastColorRed == false);

		object->requestTextureUpdate();
	}
}

void testObjectCT(Game_Object* object, Game_Rect bounds, SDL_Renderer* renderer)
{
	// Test rendering
	SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
	SDL_RenderClear(renderer);

	SDL_Rect rect;
	rect.x = 0;
	rect.y = 0;
	rect.w = bounds.width;
	rect.h = bounds.height;

	int color = object->getProperty("color", 0xFF00);
	int r = color & 0xFF;
	int g = (color & 0xFF00) / 256;
	int b = (color & 0xFF0000) / (256 * 256);

	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	SDL_RenderFillRect(renderer, &rect);
}

void testObjectKeyTyped(Game_Object* object, SDL_Event* eventData)
{
	int direction = object->getProperty("direction", 0);
	int newDirection = direction;

	SDL_KeyboardEvent* event = (SDL_KeyboardEvent*) eventData;
	bool keyDown = (event->type == SDL_KEYDOWN);
	switch (event->keysym.sym)
	{
		case SDLK_w:
			if (keyDown)
				newDirection |= 1;
			else
				newDirection ^= 1;
			break;
		case SDLK_a:
			if (keyDown)
				newDirection |= 2;
			else
				newDirection ^= 2;
			break;
		case SDLK_s:
			if (keyDown)
				newDirection |= 4;
			else
				newDirection ^= 4;
			break;
		case SDLK_d:
			if (keyDown)
				newDirection |= 8;
			else
				newDirection ^= 8;
			break;
	}

	if (newDirection != direction)
		object->setProperty("direction", newDirection);

	if (keyDown)
	{
		int color = object->getProperty("color", 0);
		int newColor = color;

		switch (event->keysym.sym)
		{
			case SDLK_1:
				newColor = 0xFF;
				break;
			case SDLK_2:
				newColor = 0xFF00;
				break;
			case SDLK_3:
				newColor = 0xFF0000;
				break;
			case SDLK_4:
				newColor = 0xFFFF;
				break;
			case SDLK_5:
				newColor = 0xFFFF00;
				break;
			case SDLK_6:
				newColor = 0xFF00FF;
				break;
			case SDLK_UP:
				Game_SharedMemory::p_zoomScale += 0.5f;
				break;
			case SDLK_DOWN:
				if (Game_SharedMemory::p_zoomScale > 0)
					Game_SharedMemory::p_zoomScale -= 0.5f;

				break;
			case SDLK_r:
				object->m_worldCoords.x = 0;
				object->m_worldCoords.y = 0;
				Game_SharedMemory::p_zoomScale = 1.0f;
				object->setProperty("flashing", 0);
				newColor = 0xFF00;
				break;
			case SDLK_f:
			{
				int flashing = object->getProperty("flashing", 0);
				object->setProperty("flashing", (flashing? 0 : 1));
				break;
			}
			case SDLK_SPACE:
			{
				int speed = object->getProperty("speed", 1);
				if ((event->keysym.mod & KMOD_LSHIFT) && speed > 0)
					speed--;
				else
					speed++;

				object->setProperty("speed", speed);
				break;
			}
		}

		if (newColor != color)
		{
			object->setProperty("color", newColor);
			object->requestTextureUpdate();
		}
	}
}

void game_run()
{
	// Start render thread
	game_startRenderThread();

	// Create game objects
	Game_Object testObject(0, 0, 100, 100);
	testObject.setFrameUpdate(testObjectFU);
	testObject.setTextureUpdate(testObjectCT);
	testObject.setEventFunction(TYPE_KEY_TYPED, testObjectKeyTyped);

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
