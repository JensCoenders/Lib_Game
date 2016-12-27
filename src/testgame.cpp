#include <iostream>
#include <sstream>
#include "testgame.h"

using namespace std;

int g_btnTextCounter = 0;

SDL_Surface* guiBtnTU(Game_Object& guiButton, Game_RenderEquipment& equipment)
{
	if (!guiButton.isModuleEnabled(game_combineModules(MODULE_IMAGE_BACKGROUND, MODULE_TEXT)))
		return NULL;

	SDL_Surface* loadedTexture = game_getAsset(guiButton.imageBackgroundModule->getTexturePath(), true);

	SDL_Rect targetRect;
	targetRect.x = 0;
	targetRect.y = 0;
	targetRect.w = guiButton.size.width;
	targetRect.h = guiButton.size.height;

	SDL_BlitScaled(loadedTexture, NULL, equipment.surface, &targetRect);

	guiButton.textModule->setText(to_string(g_btnTextCounter));
	SDL_Surface* renderedText = guiButton.textModule->renderText();
	targetRect.x = (guiButton.size.width - renderedText->w) / 2;
	targetRect.y = (guiButton.size.height - renderedText->h) / 2;
	targetRect.w = renderedText->w;
	targetRect.h = renderedText->h;

	SDL_BlitScaled(renderedText, NULL, equipment.surface, &targetRect);

	return equipment.surface;
}

void guiBtnMouseClicked(Game_Object& guiButton, Game_ObjectEvent& eventData)
{
	if (!guiButton.isModuleEnabled(MODULE_IMAGE_BACKGROUND))
		return;

	Game_MouseClickedEvent& mouseClickedEvent = (Game_MouseClickedEvent&) eventData;
	if (mouseClickedEvent.pressed)
	{
		guiButton.imageBackgroundModule->setTexturePath(game_getAssetPath("textures", "gui", "gui_btn_pressed.png"));
		return;
	}
	else if (!guiButton.eventModule->mouseHovering)
	{
		guiButton.imageBackgroundModule->setTexturePath(game_getAssetPath("textures", "gui", "gui_btn_normal.png"));
		return;
	}

	if (mouseClickedEvent.button == 1)
		g_btnTextCounter++;
	else if (mouseClickedEvent.button == 3)
		g_btnTextCounter--;

	guiButton.imageBackgroundModule->setTexturePath(game_getAssetPath("textures", "gui", "gui_btn_highlighted.png"));
}

void guiBtnMouseHoveredFunc(Game_Object& guiButton, Game_ObjectEvent& eventData)
{
	if (!guiButton.isModuleEnabled(MODULE_IMAGE_BACKGROUND))
		return;

	Game_MouseHoveredEvent& mouseHoveredEvent = (Game_MouseHoveredEvent&) eventData;
	if (mouseHoveredEvent.entering)
		guiButton.imageBackgroundModule->setTexturePath(game_getAssetPath("textures", "gui", "gui_btn_highlighted.png"));
	else
		guiButton.imageBackgroundModule->setTexturePath(game_getAssetPath("textures", "gui", "gui_btn_normal.png"));
}

SDL_Surface* tileTU(Game_Object& tile, Game_RenderEquipment& equipment)
{
	if (!tile.isModuleEnabled(game_combineModules(MODULE_IMAGE_BACKGROUND, MODULE_PROPERTY, MODULE_TEXT)))
		return NULL;

	// Clear surface
	SDL_SetRenderDrawColor(equipment.softwareRenderer, 255, 255, 255, 255);
	SDL_RenderClear(equipment.softwareRenderer);

	// Get background image
	SDL_Surface* backgroundImage = game_getAsset(tile.imageBackgroundModule->getTexturePath(), true);
	if (!backgroundImage)
		return NULL;

	SDL_BlitScaled(backgroundImage, NULL, equipment.surface, NULL);

	// Get board number text
	int boardNumber = tile.propertyModule->getIntProperty("BOARD_NUMBER", 0);
	tile.textModule->setText(to_string(boardNumber));
	SDL_Surface* boardNumberText = tile.textModule->renderText();

	SDL_Rect targetRect;
	targetRect.x = (tile.size.width - boardNumberText->w) / 2;
	targetRect.y = (tile.size.height - boardNumberText->h) / 2;
	targetRect.w = boardNumberText->w;
	targetRect.h = boardNumberText->h;
	SDL_BlitSurface(boardNumberText, NULL, equipment.surface, &targetRect);

	SDL_FreeSurface(boardNumberText);
	return equipment.surface;
}

void runTestGame()
{
	// Set game properties
	gameVar_keyboardMovesCamera = false;
	gameVar_useFpsCounter = false;

	// Generate GUI
	Game_Object guiButton = Game_Object(10, 40, 100, 60, true, game_combineModules(MODULE_EVENT, MODULE_IMAGE_BACKGROUND, MODULE_TEXT));
	guiButton.setTextureUpdate(guiBtnTU);
	guiButton.imageBackgroundModule->setTexturePath(game_getAssetPath("textures", "gui", "gui_btn_normal.png"));
	guiButton.eventModule->setEventFunction(EVENT_TYPE_CLICKED, guiBtnMouseClicked);
	guiButton.eventModule->setEventFunction(EVENT_TYPE_MOTION, guiBtnMouseHoveredFunc);
	game_addGameObject(&guiButton, GAME_LAYER_GUI_FOREGROUND);

	GAME_DEBUG_CHECK
		cout << "[DEBUG] Created game objects" << endl;

	// Run main loop
	game_runMainLoop();
}
