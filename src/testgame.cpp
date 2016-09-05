#include <iostream>
#include <sstream>
#include "testgame.h"

using namespace std;

void speedLabFU(Game_Object& object)
{
	if (!object.isModuleEnabled(MODULE_TEXT))
		return;

	static int lastMovementSpeed = 0;
	if (lastMovementSpeed != game_shmGet(SHM_WORLD_MAIN_CAMERA).movementSpeed)
	{
		lastMovementSpeed = game_shmGet(SHM_WORLD_MAIN_CAMERA).movementSpeed;

		ostringstream stream;
		stream << "Camera movement speed: " << lastMovementSpeed;
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
	game_shmPut(SHM_WORLD_KEYBOARD_MOVES_CAMERA, true);

	// Debug objects
	Game_Object fpsObject(5, -51, 0, 0, true, MODULE_TEXT);
	fpsObject.setTextureUpdate(textObjectTextureUpdate);
	fpsObject.textModule->setText("FPS: 0");
	game_shmPut(SHM_MISC_FPS_OBJECT, &fpsObject);

	Game_Object speedLab(5, -26, 0, 0, true, MODULE_TEXT);
	speedLab.setTextureUpdate(textObjectTextureUpdate);
	speedLab.setFrameUpdate(speedLabFU);

	Game_Object zoomScaleLab(5, -1, 0, 0, true, MODULE_TEXT);
	zoomScaleLab.setTextureUpdate(textObjectTextureUpdate);
	zoomScaleLab.setFrameUpdate(zoomScaleLabFU);

	game_addGameObject(&fpsObject, GAME_LAYER_GUI_BACKGROUND);
	game_addGameObject(&speedLab, GAME_LAYER_GUI_BACKGROUND);
	game_addGameObject(&zoomScaleLab, GAME_LAYER_GUI_BACKGROUND);

	// Game objects
	Game_Object background(0, 0, -1, -1, true, MODULE_IMAGE_BACKGROUND);
	background.setTextureUpdate(imageTextureObjectTU);
	background.imageBackgroundModule->setTexturePath(game_getAssetPath("background.png", "textures"));

	game_addGameObject(&background, GAME_LAYER_LEVEL_BACKGROUND);

	// Main loop
	game_runMainLoop();
}
