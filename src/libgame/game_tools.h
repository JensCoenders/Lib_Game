#ifndef GAME_TOOLS_H
#define GAME_TOOLS_H

#include <string>
#include <SDL2/SDL_ttf.h>

#include "game_object.h"
#include "game_shm.h"
#include "game_types.h"

/* Render functions */

// Objects
bool game_addGameObject(Game_Object* object, int layerID);
bool game_removeGameObject(Game_Object* object);

// Misc
Game_Rect game_getTextSize(std::string text, TTF_Font* font = game_shmGet(SHM_MISC_GUI_FONT));
Game_RenderEquipment* game_createRenderEquipment(int surfaceWidth, int surfaceHeight);

/* Misc functions */
Game_Object* game_findObjectByID(unsigned int objectID, Game_RenderLayer** outputLayer = NULL, LinkedListNode<
        Game_Object>** outputNode = NULL);

SDL_Surface* imageTextureObjectTU(Game_Object& object, Game_RenderEquipment* equipment);
SDL_Surface* textObjectTextureUpdate(Game_Object& object, Game_RenderEquipment* equipment);

/* Template combine functions */
std::string combineStringPath(std::string firstString = "");

template<typename First, typename ... Rest>
std::string combineStringPath(First firstString, Rest&... rest)
{
	return std::string(firstString) + "\\" + combineStringPath(rest...);
}

template<typename First, typename ... Rest>
std::string game_getAssetPath(First name, Rest&... subDirs)
{
	return game_shmGet(SHM_MISC_ASSETS_DIR) + "\\" + combineStringPath(subDirs...) + "\\" + std::string(name);
}

#endif
