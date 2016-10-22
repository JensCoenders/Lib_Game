#include <iostream>
#include <sstream>
#include "testgame.h"

using namespace std;

void playerFU(Game_Object& object)
{
	unsigned char movementDirection = gameVar_mainCamera.movementDirection;
	int movementSpeed = gameVar_mainCamera.movementSpeed;

	if (!movementDirection || gameVar_keyboardMovesCamera)
		return;

	if (movementDirection & 0x1)
	{
		object.position.y -= movementSpeed;
		if (movementDirection & 0x4)
		{
			object.position.x -= movementSpeed;
			object.rotation = 315;
		}
		else if (movementDirection & 0x8)
		{
			object.position.x += movementSpeed;
			object.rotation = 45;
		}
		else
			object.rotation = 0;
	}
	else if (movementDirection & 0x2)
	{
		object.position.y += movementSpeed;
		if (movementDirection & 0x4)
		{
			object.position.x -= movementSpeed;
			object.rotation = 225;
		}
		else if (movementDirection & 0x8)
		{
			object.position.x += movementSpeed;
			object.rotation = 135;
		}
		else
			object.rotation = 180;
	}
	else if (movementDirection & 0x4)
	{
		object.position.x -= movementSpeed;
		object.rotation = 270;
	}
	else if (movementDirection & 0x8)
	{
		object.position.x += movementSpeed;
		object.rotation = 90;
	}
}

void runTestGame()
{
	// Set game properties
	gameVar_keyboardMovesCamera = false;
	gameVar_useFpsCounter = true;

	Game_Object background(0, 0, 0, 0, false, game_combineModules(MODULE_IMAGE_BACKGROUND, MODULE_EXTRA_BOUNDS));
	background.setTextureUpdate(imageTextureObjectTU);
	background.imageBackgroundModule->setTexturePath(game_getAssetPath("background_grass.png", "textures"));
	background.extraBoundsModule->fillScreen = true;

	Game_Object obstacle(400, 300, 100, 120, false, MODULE_IMAGE_BACKGROUND);
	obstacle.setTextureUpdate(imageTextureObjectTU);
	obstacle.imageBackgroundModule->setTexturePath(game_getAssetPath("obstacle.png", "textures"));

	Game_Object player(0, 0, 100, 100, false, MODULE_IMAGE_BACKGROUND);
	player.setFrameUpdate(playerFU);
	player.setTextureUpdate(imageTextureObjectTU);
	player.imageBackgroundModule->setTexturePath(game_getAssetPath("player.png", "textures"));
	gameVar_mainCamera.centeredObject = &player;

	game_addGameObject(&obstacle, GAME_LAYER_LEVEL_MID_1);
	game_addGameObject(&player, GAME_LAYER_LEVEL_FOREGROUND);
	game_addGameObject(&background, GAME_LAYER_LEVEL_BACKGROUND);

	GAME_DEBUG_CHECK
		cout << "[DEBUG] Created game objects" << endl;

	// Main loop
	cout << "[INFO] Successfully initialized game" << endl;
	game_runMainLoop();
}
