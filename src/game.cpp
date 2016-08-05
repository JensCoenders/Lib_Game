#include <iostream>
#include "game.h"

void testObjectFU(Game_Object* object)
{
	Game_WorldObject* worldObject = (Game_WorldObject*) object;

	int direction = object->getIntProperty("direction", 0);
	int speed = object->getIntProperty("speed", 1);

	int newX = worldObject->getCoords().x;
	int newY = worldObject->getCoords().y;
	if ((direction & 1) == 1)
		newY -= speed;
	if ((direction & 4) == 4)
		newY += speed;

	if ((direction & 2) == 2)
		newX -= speed;
	if ((direction & 8) == 8)
		newX += speed;

	worldObject->setCoords(newX, newY);

	if (worldObject->getBoolProperty("flashing", false))
	{
		static bool lastColorRed = false;
		if (lastColorRed)
			object->setProperty("color", 0xFF00);
		else
			object->setProperty("color", 0xFF);

		lastColorRed = (lastColorRed == false);

		worldObject->requestTextureUpdate();
	}
}

void testObjectCT(Game_Object* object, SDL_Surface* surface, SDL_Renderer* renderer)
{
	SDL_Rect destRect;
	destRect.x = 0;
	destRect.y = 0;
	destRect.w = surface->w;
	destRect.h = surface->h;

	int color = object->getIntProperty("color", 0xFF00);
	int r = color & 0xFF;
	int g = (color & 0xFF00) / 256;
	int b = (color & 0xFF0000) / (256 * 256);

	SDL_SetRenderDrawColor(renderer, r, g, b, 255);
	SDL_RenderFillRect(renderer, &destRect);
}

void testObjectKeyTyped(Game_Object* object, SDL_Event* eventData)
{
	int direction = object->getIntProperty("direction", 0);
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
		int color = object->getIntProperty("color", 0);
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
				object->setCoords(0, 0);
				Game_SharedMemory::p_zoomScale = 1.0f;
				object->setProperty("flashing", false);
				object->setProperty("speed", 1);
				newColor = 0xFF00;
				break;
			case SDLK_f:
			{
				int flashing = object->getBoolProperty("flashing", false);
				object->setProperty("flashing", (flashing == false));
				break;
			}
			case SDLK_SPACE:
			{
				int speed = object->getIntProperty("speed", 1);
				if ((event->keysym.mod & KMOD_LSHIFT) && speed > 1)
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
	testObject.setEventFunction(EVENT_TYPE_KEY, testObjectKeyTyped);

	Game_SharedMemory::startRenderingObject(&testObject, GAME_LAYER_LEVEL_FOREGROUND);
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
	delete Game_SharedMemory::m_fpsObject;

	cout << "[INFO] Stopped main thread" << endl;
}
