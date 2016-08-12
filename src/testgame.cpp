#include <iostream>
#include <sstream>
#include "testgame.h"

SDL_Surface* obstacleTU(Game_Object& object, Game_RenderEquipment* equipment)
{
	SDL_Rect destRect;
	destRect.x = 0;
	destRect.y = 0;
	destRect.w = equipment->surface->w;
	destRect.h = equipment->surface->h;

	SDL_Color backgroundColor = object.getBackgroundColor();

	SDL_SetRenderDrawColor(equipment->softwareRenderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, 255);
	SDL_RenderFillRect(equipment->softwareRenderer, &destRect);

	return equipment->surface;
}

void playerFU(Game_Object& object)
{
	Game_Camera& mainCamera = Game_SharedMemory::w_mainCamera;
	int newX = 0, newY = 0;
	newX = mainCamera.m_position.x + (mainCamera.m_size.width - object.getSize().width) / 2;
	newY = mainCamera.m_position.y + (mainCamera.m_size.height - object.getSize().height) / 2;
	object.setCoords(newX, newY);
}

void playerClicked(Game_AdvancedObject& object, SDL_Event& eventData)
{
	SDL_MouseButtonEvent& event = eventData.button;
	if (event.state == SDL_PRESSED)
	{
		static int colorIndex = 0;
		switch (event.button)
		{
			case SDL_BUTTON_LEFT:
				colorIndex++;
				break;
			case SDL_BUTTON_RIGHT:
				colorIndex--;
				break;
			default:
				break;
		}

		if (colorIndex < 0)
			colorIndex = 2;
		else if (colorIndex > 2)
			colorIndex = 0;

		SDL_Color newColor = {0, 0, 0, 255};
		switch (colorIndex)
		{
			case 0:
				newColor.r = 255;
				break;
			case 1:
				newColor.g = 255;
				break;
			case 2:
				newColor.b = 255;
				break;
		}

		object.setBackgroundColor(newColor);
	}
}

void speedLabFU(Game_Object& object)
{
	Game_AdvancedObject& advancedObject = (Game_AdvancedObject&) object;

	int lastMovementSpeed = advancedObject.getIntProperty("lastMovementSpeed", 0);
	if (lastMovementSpeed != Game_SharedMemory::w_mainCamera.m_movementSpeed)
	{
		lastMovementSpeed = Game_SharedMemory::w_mainCamera.m_movementSpeed;
		advancedObject.setProperty("lastMovementSpeed", lastMovementSpeed);

		ostringstream stream;
		stream << "Camera movement speed: " << lastMovementSpeed;
		advancedObject.setText(stream.str());
	}
}

void zoomScaleLabFU(Game_Object& object)
{
	static double lastZoomScale = 0;
	if (lastZoomScale != Game_SharedMemory::w_zoomScale)
	{
		lastZoomScale = Game_SharedMemory::w_zoomScale;

		ostringstream stream;
		stream << "Zoom scale: " << lastZoomScale;
		((Game_AdvancedObject&) object).setText(stream.str());
	}
}

void runTestGame()
{
	// Create game objects
	SDL_Color* colorList = new SDL_Color[5] { {255, 0, 0}, {0, 255, 0}, {0, 0, 255}, {0, 255, 255}, {255, 0, 255}};
	Game_Object** obstacles = new Game_Object*[10];
	for (int i = 0; i < 10; i++)
	{
		Game_Object* obstacle = new Game_Object(10 + i * 70, 10, 50, 50, OBJECT_TYPE_WORLD);
		obstacle->setTextureUpdate(obstacleTU);
		obstacle->setBackgroundColor(colorList[i % 5]);

		obstacles[i] = obstacle;
		Game_Tools::addGameObject(obstacle, GAME_LAYER_LEVEL_MID_1);
	}

	delete[] colorList;

	Game_AdvancedObject player(0, 0, 80, 40, OBJECT_TYPE_WORLD);
	player.setFrameUpdate(playerFU);
	player.setTextureUpdate(obstacleTU);
	player.setEventFunction(EVENT_TYPE_CLICKED, playerClicked);
	player.setBackgroundColor( {255, 0, 0});

	Game_AdvancedObject speedLab(0, 25, 275, 50, OBJECT_TYPE_GUI);
	speedLab.setFrameUpdate(speedLabFU);

	Game_AdvancedObject zoomScaleLab(0, 50, 150, 50, OBJECT_TYPE_GUI);
	zoomScaleLab.setFrameUpdate(zoomScaleLabFU);

	Game_Tools::addGameObject(&player, GAME_LAYER_LEVEL_FOREGROUND);
	Game_Tools::addGameObject(&speedLab, GAME_LAYER_GUI_FOREGROUND);
	Game_Tools::addGameObject(&zoomScaleLab, GAME_LAYER_GUI_FOREGROUND);

	// Main loop
	game_mainLoop();

	// Delete objects
	for (int i = 0; i < 5; i++)
		delete obstacles[i];
	delete[] obstacles;

	cout << "[INFO] Stopped main thread" << endl;
}
