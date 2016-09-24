#include <iostream>
#include <sstream>
#include "testgame.h"

using namespace std;

void speedLabFU(Game_Object& object)
{
	if (!object.isModuleEnabled(MODULE_TEXT))
		return;

	static int lastMovementSpeed = 0;
	string prefixText;
	if (!game_shmGet(SHM_WORLD_KEYBOARD_MOVES_CAMERA) && game_shmGet(SHM_WORLD_MAIN_CAMERA).centeredObject)
		prefixText = "Centered object movement speed: ";
	else
		prefixText = "Main camera movement speed: ";

	if (lastMovementSpeed != game_shmGet(SHM_WORLD_MAIN_CAMERA).movementSpeed)
	{
		lastMovementSpeed = game_shmGet(SHM_WORLD_MAIN_CAMERA).movementSpeed;

		ostringstream stream;
		stream << prefixText << lastMovementSpeed;
		object.textModule->setText(stream.str());
	}

}

void zoomScaleLabFU(Game_Object& object)
{
	if (!object.isModuleEnabled(MODULE_TEXT))
		return;

	static double lastZoomScale = 0;
	if (lastZoomScale != game_shmGet(SHM_WORLD_ZOOM_SCALE))
	{
		lastZoomScale = game_shmGet(SHM_WORLD_ZOOM_SCALE);

		ostringstream stream;
		stream << "Zoom scale: " << lastZoomScale;
		object.textModule->setText(stream.str());
	}
}

void playerPosLabFU(Game_Object& object)
{
	if (!object.isModuleEnabled(MODULE_TEXT))
		return;

	static Game_Point lastObjectPos = {-1, -1};
	Game_Point newObjectPos = game_shmGet(SHM_WORLD_MAIN_CAMERA).centeredObject->position;
	if (lastObjectPos.x != newObjectPos.x || lastObjectPos.y != newObjectPos.y)
	{
		lastObjectPos = newObjectPos;

		ostringstream stream;
		stream << "Player: " << newObjectPos.toString();
		object.textModule->setText(stream.str());
	}
}

void playerFU(Game_Object& object)
{
	Game_Camera* mainCamera = &game_shmGet(SHM_WORLD_MAIN_CAMERA);
	if (!mainCamera->movementDirection)
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
	// Initialize game
	game_shmPut(SHM_WORLD_KEYBOARD_MOVES_CAMERA, false);
	game_shmPut(SHM_GAME_USE_FPS_COUNTER, true);

	// Debug objects
	GAME_DEBUG_CHECK
	{
		SDL_Color debugColor = {255, 255, 0, 255};

		Game_Object speedLab(5, 25, 0, 0, true, MODULE_TEXT);
		speedLab.setFrameUpdate(speedLabFU);
		speedLab.setTextureUpdate(textObjectTextureUpdate);
		speedLab.textModule->setTextColor(debugColor);

		Game_Object zoomScaleLab(5, 50, 0, 0, true, MODULE_TEXT);
		zoomScaleLab.setFrameUpdate(zoomScaleLabFU);
		zoomScaleLab.setTextureUpdate(textObjectTextureUpdate);
		zoomScaleLab.textModule->setTextColor(debugColor);

		Game_Object playerPosLab(5, 74, 0, 0, true, MODULE_TEXT);
		playerPosLab.setFrameUpdate(playerPosLabFU);
		playerPosLab.setTextureUpdate(textObjectTextureUpdate);
		playerPosLab.textModule->setTextColor(debugColor);

		game_addGameObject(&speedLab, GAME_LAYER_GUI_FOREGROUND);
		game_addGameObject(&zoomScaleLab, GAME_LAYER_GUI_FOREGROUND);
		game_addGameObject(&playerPosLab, GAME_LAYER_GUI_FOREGROUND);

		cout << "[DEBUG] Created debug objects" << endl;
	}

	// Game objects
	Game_Object background(0, 0, -1, -1, false, MODULE_IMAGE_BACKGROUND);
	background.setTextureUpdate(imageTextureObjectTU);
	background.imageBackgroundModule->setTexturePath(game_getAssetPath("background_grass.png", "textures"));

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
