#include <iostream>
#include <sstream>

#include "testgame.h"

void playerFU(Game_Object& object)
{
	Game_Camera& mainCamera = Game_SharedMemory::w_mainCamera;
	object.worldCoords.x = mainCamera.position.x + (mainCamera.size.width - object.worldSize.width) / 2;
	object.worldCoords.y = mainCamera.position.y + (mainCamera.size.height - object.worldSize.height) / 2;
}

void speedLabFU(Game_Object& object)
{
	Game_TextObject& advancedObject = (Game_TextObject&) object;

	static int lastMovementSpeed = 0;
	if (lastMovementSpeed != Game_SharedMemory::w_mainCamera.movementSpeed)
	{
		lastMovementSpeed = Game_SharedMemory::w_mainCamera.movementSpeed;

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
		((Game_TextObject&) object).setText(stream.str());
	}
}

void runTestGame()
{
	// Create game objects
	Game_Object* obstacles[10];
	for (int i = 0; i < 10; i++)
	{
		Game_Object* obstacle = new Game_Object(10 + i * 110, 10, 100, 100, false);
		obstacle->setTextureUpdate(Game_Tools::imageTextureObjectTU);
		obstacle->renderPars = new Game_RP_ImageTexture(Game_Tools::getAssetPath("container_circle.png", "textures"));

		obstacles[i] = obstacle;
		Game_Tools::addGameObject(obstacle, GAME_LAYER_LEVEL_MID_1);
	}

	Game_Object background(0, 0, -1, -1, true);
	background.setTextureUpdate(Game_Tools::imageTextureObjectTU);
	background.renderPars = new Game_RP_ImageTexture(Game_Tools::getAssetPath("background.png", "textures"));

	Game_Object player(0, 0, 100, 100, false);
	player.setFrameUpdate(playerFU);
	player.setTextureUpdate(Game_Tools::imageTextureObjectTU);
	player.renderPars = new Game_RP_ImageTexture(Game_Tools::getAssetPath("player.png", "textures"));

	Game_TextObject speedLab(0, 25, 275, 50, true);
	speedLab.setFrameUpdate(speedLabFU);

	Game_TextObject zoomScaleLab(0, 50, 150, 50, true);
	zoomScaleLab.setFrameUpdate(zoomScaleLabFU);

	Game_Tools::addGameObject(&background, GAME_LAYER_LEVEL_BACKGROUND);
	Game_Tools::addGameObject(&player, GAME_LAYER_LEVEL_FOREGROUND);
	Game_Tools::addGameObject(&speedLab, GAME_LAYER_GUI_FOREGROUND);
	Game_Tools::addGameObject(&zoomScaleLab, GAME_LAYER_GUI_FOREGROUND);

	// Main loop
	game_mainLoop();

	// Delete objects
	for (int i = 0; i < 5; i++)
		delete obstacles[i];

	cout << "[INFO] Stopped main thread" << endl;
}
