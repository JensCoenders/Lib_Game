#include <iostream>
#include <sstream>
#include "testgame.h"

void obstacleCT(Game_Object* object, SDL_Surface* surface, SDL_Renderer* renderer)
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

void playerFU(Game_Object* object)
{
	Game_Camera& mainCamera = Game_SharedMemory::w_mainCamera;
	int newX = 0, newY = 0;
	newX = mainCamera.m_position.x + (mainCamera.m_size.width - object->getSize().width) / 2;
	newY = mainCamera.m_position.y + (mainCamera.m_size.height - object->getSize().height) / 2;
	object->setCoords(newX, newY);
}

void speedLabFU(Game_Object* object)
{
	static int lastMovementSpeed = 0;
	if (lastMovementSpeed != Game_SharedMemory::w_mainCamera.m_movementSpeed)
	{
		lastMovementSpeed = Game_SharedMemory::w_mainCamera.m_movementSpeed;

		ostringstream stream;
		stream << "Camera movement speed: " << lastMovementSpeed;
		((Game_GUIObject*) object)->setText(stream.str());
	}
}

void zoomScaleLabFU(Game_Object* object)
{
	static float lastZoomScale = 0;
	if (lastZoomScale != Game_SharedMemory::w_zoomScale)
	{
		lastZoomScale = Game_SharedMemory::w_zoomScale;

		ostringstream stream;
		stream << "Zoom scale: " << lastZoomScale;
		((Game_GUIObject*) object)->setText(stream.str());
	}
}

void runTestGame()
{
	// Create game objects
	Game_WorldObject** obstacles = new Game_WorldObject*[5];
	for (int i = 0; i < 5; i++)
	{
		Game_WorldObject* obstacle = new Game_WorldObject(i * 75 + 337, 200, 50, 50);
		obstacle->setTextureUpdate(obstacleCT);

		int newColor = 0xEE;
		for (int j = 0; j < i; j++)
			newColor *= newColor;

		obstacle->setProperty("color", newColor);

		obstacles[i] = obstacle;
		Game_SharedMemory::addGameObject(obstacle, GAME_LAYER_LEVEL_MID_1);
	}

	Game_WorldObject* player = new Game_WorldObject(0, 0, 25, 25);
	player->setFrameUpdate(playerFU);
	player->setTextureUpdate(obstacleCT);
	player->setProperty("color", 0xFFFFFF);

	Game_GUIObject* speedLab = new Game_GUIObject(0, 25, 275, 50);
	speedLab->setFrameUpdate(speedLabFU);

	Game_GUIObject* zoomScaleLab = new Game_GUIObject(0, 50, 150, 50);
	zoomScaleLab->setFrameUpdate(zoomScaleLabFU);

	Game_SharedMemory::addGameObject(player, GAME_LAYER_LEVEL_FOREGROUND);
	Game_SharedMemory::addGameObject(speedLab, GAME_LAYER_GUI_FOREGROUND);
	Game_SharedMemory::addGameObject(zoomScaleLab, GAME_LAYER_GUI_FOREGROUND);

	// Main loop
	game_mainLoop();

	// Delete objects
	for (int i = 0; i < 5; i++)
	{
		delete obstacles[i];
	}
	delete[] obstacles;
	delete player;
	delete speedLab;
	delete zoomScaleLab;

	cout << "[INFO] Stopped main thread" << endl;
}
