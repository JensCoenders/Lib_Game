#include <iostream>
#include <sstream>
#include "testgame.h"

using namespace std;

void playerFU(Game_Object& object)
{
	Game_Camera* mainCamera = &game_shmGet(SHM_WORLD_MAIN_CAMERA);
	if (!mainCamera->movementDirection || game_shmGet(SHM_WORLD_KEYBOARD_MOVES_CAMERA))
		return;

	if (mainCamera->movementDirection & 0x1)
	{
		object.position.y -= mainCamera->movementSpeed;
		if (mainCamera->movementDirection & 0x4)
		{
			object.position.x -= mainCamera->movementSpeed;
			object.rotation = 315;
		}
		else if (mainCamera->movementDirection & 0x8)
		{
			object.position.x += mainCamera->movementSpeed;
			object.rotation = 45;
		}
		else
			object.rotation = 0;
	}
	else if (mainCamera->movementDirection & 0x2)
	{
		object.position.y += mainCamera->movementSpeed;
		if (mainCamera->movementDirection & 0x4)
		{
			object.position.x -= mainCamera->movementSpeed;
			object.rotation = 225;
		}
		else if (mainCamera->movementDirection & 0x8)
		{
			object.position.x += mainCamera->movementSpeed;
			object.rotation = 135;
		}
		else
			object.rotation = 180;
	}
	else if (mainCamera->movementDirection & 0x4)
	{
		object.position.x -= mainCamera->movementSpeed;
		object.rotation = 270;
	}
	else if (mainCamera->movementDirection & 0x8)
	{
		object.position.x += mainCamera->movementSpeed;
		object.rotation = 90;
	}
}

void runTestGame()
{
	// Set game properties
	game_shmPut(SHM_WORLD_KEYBOARD_MOVES_CAMERA, false);
	game_shmPut(SHM_GAME_USE_FPS_COUNTER, true);

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
	game_shmGet(SHM_WORLD_MAIN_CAMERA).centeredObject = &player;

	game_addGameObject(&obstacle, GAME_LAYER_LEVEL_MID_1);
	game_addGameObject(&player, GAME_LAYER_LEVEL_FOREGROUND);
	game_addGameObject(&background, GAME_LAYER_LEVEL_BACKGROUND);

	GAME_DEBUG_CHECK
		cout << "[DEBUG] Created game objects" << endl;

	// Main loop
	cout << "[INFO] Successfully initialized game" << endl;
	game_runMainLoop();
}
