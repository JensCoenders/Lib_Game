#include <iostream>
#include <sstream>
#include "testgame.h"

SDL_Surface* obstacleCT(Game_Object* object, Game_RenderEquipment* equipment)
{
	SDL_Rect destRect;
	destRect.x = 0;
	destRect.y = 0;
	destRect.w = equipment->surface->w;
	destRect.h = equipment->surface->h;

	int color = object->getIntProperty("color", 0xFF00);
	int r = color & 0xFF;
	int g = (color & 0xFF00) / 256;
	int b = (color & 0xFF0000) / (256 * 256);

	SDL_SetRenderDrawColor(equipment->softwareRenderer, r, g, b, 255);
	SDL_RenderFillRect(equipment->softwareRenderer, &destRect);

	return equipment->surface;
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
	int lastMovementSpeed = object->getIntProperty("lastMovementSpeed", 0);

	if (lastMovementSpeed != Game_SharedMemory::w_mainCamera.m_movementSpeed)
	{
		lastMovementSpeed = Game_SharedMemory::w_mainCamera.m_movementSpeed;
		object->setProperty("lastMovementSpeed", lastMovementSpeed);

		ostringstream stream;
		stream << "Camera movement speed: " << lastMovementSpeed;
		((Game_TextObject*) object)->setText(stream.str());
	}
}

void zoomScaleLabFU(Game_Object* object)
{
	static double lastZoomScale = 0;
	if (lastZoomScale != Game_SharedMemory::w_zoomScale)
	{
		lastZoomScale = Game_SharedMemory::w_zoomScale;

		ostringstream stream;
		stream << "Zoom scale: " << lastZoomScale;
		((Game_TextObject*) object)->setText(stream.str());
	}
}

void runTestGame()
{
	// Create game objects
	Game_TextObject** obstacles = new Game_TextObject*[5];
	for (int i = 0; i < 5; i++)
	{
		Game_TextObject* obstacle = new Game_TextObject(i * 75 + 337, 200, 50, 50, OBJECT_TYPE_WORLD);
		obstacle->setTextureUpdate(obstacleCT);

		int newColor = 0xEE;
		for (int j = 0; j < i; j++)
			newColor *= newColor;

		obstacle->setProperty("color", newColor);

		obstacles[i] = obstacle;
		Game_Tools::addGameObject(obstacle, GAME_LAYER_LEVEL_MID_1);
	}

	Game_Object* player = new Game_Object(0, 0, 25, 25, OBJECT_TYPE_WORLD);
	player->setFrameUpdate(playerFU);
	player->setTextureUpdate(obstacleCT);
	player->setProperty("color", 0xFFFFFF);

	Game_TextObject* speedLab = new Game_TextObject(0, 25, 275, 50, OBJECT_TYPE_GUI);
	speedLab->setFrameUpdate(speedLabFU);

	Game_TextObject* zoomScaleLab = new Game_TextObject(0, 50, 150, 50, OBJECT_TYPE_GUI);
	zoomScaleLab->setFrameUpdate(zoomScaleLabFU);

	Game_Tools::addGameObject(player, GAME_LAYER_LEVEL_FOREGROUND);
	Game_Tools::addGameObject(speedLab, GAME_LAYER_GUI_FOREGROUND);
	Game_Tools::addGameObject(zoomScaleLab, GAME_LAYER_GUI_FOREGROUND);

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
