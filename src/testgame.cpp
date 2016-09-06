#include <iostream>
#include <sstream>
#include "testgame.h"

using namespace std;

void speedLabFU(Game_Object& object)
{
	if (!object.isModuleEnabled(MODULE_TEXT))
		return;

	static int lastMovementSpeed = 0;
	int newMovementSpeed = 0;
	string prefixText;
	if (game_shmGet(SHM_WORLD_CENTERED_OBJECT) && !game_shmGet(SHM_WORLD_KEYBOARD_MOVES_CAMERA))
	{
		newMovementSpeed = game_shmGet(SHM_WORLD_CENTERED_OBJECT_MOVEMENT).speed;
		prefixText = "Centered object movement speed: ";
	}
	else
	{
		newMovementSpeed = game_shmGet(SHM_WORLD_MAIN_CAMERA).movementPars.speed;
		prefixText = "Main camera movement speed: ";
	}

	if (lastMovementSpeed != newMovementSpeed)
	{
		lastMovementSpeed = newMovementSpeed;

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

void runTestGame()
{
	// Initialize game
	game_shmPut(SHM_WORLD_KEYBOARD_MOVES_CAMERA, false);

	SDL_Color yellow = {255, 255, 0, 255};

	// Debug objects
	Game_Object fpsObject(5, 0, 0, 0, true, MODULE_TEXT);
	fpsObject.setTextureUpdate(textObjectTextureUpdate);
	fpsObject.textModule->setText("FPS: 0");
	fpsObject.textModule->setTextColor(yellow);
	game_shmPut(SHM_MISC_FPS_OBJECT, &fpsObject);

	Game_Object speedLab(5, 25, 0, 0, true, MODULE_TEXT);
	speedLab.setTextureUpdate(textObjectTextureUpdate);
	speedLab.textModule->setTextColor(yellow);
	speedLab.setFrameUpdate(speedLabFU);

	Game_Object zoomScaleLab(5, 50, 0, 0, true, MODULE_TEXT);
	zoomScaleLab.setTextureUpdate(textObjectTextureUpdate);
	zoomScaleLab.textModule->setTextColor(yellow);
	zoomScaleLab.setFrameUpdate(zoomScaleLabFU);

	game_addGameObject(&fpsObject, GAME_LAYER_GUI_BACKGROUND);
	game_addGameObject(&speedLab, GAME_LAYER_GUI_BACKGROUND);
	game_addGameObject(&zoomScaleLab, GAME_LAYER_GUI_BACKGROUND);

	// Game objects
	Game_Object background(0, 0, -1, -1, true, MODULE_IMAGE_BACKGROUND);
	background.setTextureUpdate(imageTextureObjectTU);
	background.imageBackgroundModule->setTexturePath(game_getAssetPath("background_grass.png", "textures"));

	Game_Object obstacle(400, 300, 100, 120, false, MODULE_IMAGE_BACKGROUND);
	obstacle.setTextureUpdate(imageTextureObjectTU);
	obstacle.imageBackgroundModule->setTexturePath(game_getAssetPath("obstacle.png", "textures"));

	Game_Object player(0, 0, 100, 100, false, MODULE_IMAGE_BACKGROUND);
	player.setTextureUpdate(imageTextureObjectTU);
	player.imageBackgroundModule->setTexturePath(game_getAssetPath("player.png", "textures"));
	game_shmPut(SHM_WORLD_CENTERED_OBJECT, &player);

	game_addGameObject(&obstacle, GAME_LAYER_LEVEL_MID_1);
	game_addGameObject(&player, GAME_LAYER_LEVEL_FOREGROUND);
	game_addGameObject(&background, GAME_LAYER_LEVEL_BACKGROUND);

	// Main loop
	game_runMainLoop();
}
