#ifndef GAME_TOOLS_H
#define GAME_TOOLS_H

#include <string>
#include <SDL2/SDL_ttf.h>
#include "game_object.h"
#include "game_shm.h"
#include "game_types.h"

/* Render functions */

// Objects
bool game_renderAddObject(Game_Object* object, int layerID);
bool game_renderRemoveObject(Game_Object* object);

// Misc
Game_RenderEquipment* game_renderCreateEquipment(int surfaceWidth, int surfaceHeight);
Game_Rect game_renderGetTextSize(std::string text, TTF_Font* font = game_shmGet(SHM_MISC_GUI_FONT));

/* Misc functions */
Game_Object* game_findObjectByID(int objectID, Game_RenderLayer** outputLayer = NULL, LinkedListNode<Game_Object>** outputNode = NULL);

SDL_Surface* imageTextureObjectTU(Game_Object& object, Game_RenderEquipment* equipment);

#endif

/* Function definitions */

template<typename T>
std::string combineStrings(T& firstString)
{
	return firstString;
}

template<typename T, typename ... Args>
std::string combineStrings(T& firstString, Args&... args)
{
	return firstString + "\\" + combineStrings(args...);
}

template<typename ... Args>
std::string game_assetGetPath(std::string name, Args&... subDirectories)
{
	return game_shmGet(SHM_MISC_ASSETS_DIR) + "\\" + combineStrings(subDirectories...) + "\\" + name;
}
