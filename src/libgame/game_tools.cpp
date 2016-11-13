#include <iostream>
#include <SDL2/SDL_image.h>

#include "game_defs.h"
#include "game_tools.h"

#include "game_global.h"
#include "game_types.h"

using namespace std;

bool game_isInside(Game_Point parentPos, Game_Rect parentSize, Game_Point childPos, Game_Rect childSize, bool forceFullyInside)
{
	// Check if child is fully inside parent
	bool xCheck = (childPos.x >= parentPos.x && (childPos.x + childSize.width) <= (parentPos.x + parentSize.width));
	bool yCheck = (childPos.y >= parentPos.y && (childPos.y + childSize.height) <= (parentPos.y + parentSize.height));
	if (xCheck && yCheck)
		return true;
	else if (forceFullyInside)
		return false;

	// Check if child crosses parent
	if (!xCheck)
		xCheck = (childPos.x < parentPos.x && (childPos.x + childSize.width) >= parentPos.x);
	if (!yCheck)
		yCheck = (childPos.y < parentPos.y && (childPos.y + childSize.height) >= parentPos.y);
	if (xCheck && yCheck)
		return true;

	// Check if child begins in parent
	if (!xCheck)
		xCheck = (childPos.x >= parentPos.x && childPos.x <= (parentPos.x + parentSize.width));
	if (!yCheck)
		yCheck = (childPos.y >= parentPos.y && childPos.y <= (parentPos.y + parentSize.height));
	if (xCheck && yCheck)
		return true;

	// Check if child ends in parent
	if (!xCheck)
	{
		xCheck = (childPos.x < parentPos.x && (childPos.x + childSize.width) >= parentPos.x && (childPos.x + childSize.width) <= (parentPos.x + parentSize.width));
	}
	if (!yCheck)
	{
		yCheck = (childPos.y < parentPos.y && (childPos.y + childSize.height) >= parentPos.y && (childPos.y + childSize.height) <= (parentPos.y + parentSize.height));
	}
	return (xCheck && yCheck);
}

bool game_isRenderPosInside(Game_Object& parent, Game_Object& child, bool forceFullyInside)
{
	return game_isInside(game_getObjectRenderPos(parent), game_getObjectRenderSize(parent),
		game_getObjectRenderPos(child), game_getObjectRenderSize(child), forceFullyInside);
}

bool game_loadAsset(string assetPath)
{
	SDL_Surface* loadedSurface = IMG_Load(assetPath.c_str());
	if (!loadedSurface)
	{
		GAME_DEBUG_CHECK
			cout << "[WARNING] Cannot load '" << assetPath << "'!" << endl;

		return false;
	}

	Game_Asset* newAsset = new Game_Asset(assetPath, loadedSurface);
	gameVar_loadedAssets.add(newAsset);
	return true;
}

bool game_assetSearchFunc(Game_Asset* currentAsset, string assetPath)
{
	for (unsigned int i = 0; i < assetPath.length(); i++)
		assetPath[i] = tolower(assetPath[i]);

	return currentAsset->assetPath == assetPath;
}

SDL_Surface* game_getAsset(string assetPath, bool loadAsset)
{
	if (!gameVar_loadedAssets.hasSearchFunc())
		gameVar_loadedAssets.setSearchFunc(game_assetSearchFunc);

	Game_Asset* targetAsset = gameVar_loadedAssets.search(assetPath);
	if (targetAsset)
		return targetAsset->loadedSurface;

	if (!loadAsset || !game_loadAsset(assetPath))
		return NULL;

	return game_getAsset(assetPath);
}

void game_freeAssets()
{
	GAME_DEBUG_CHECK
		cout << "[DEBUG] Freeing assets... ";

	gameVar_loadedAssets.removeAll();

	GAME_DEBUG_CHECK
		cout << "[OK]" << endl;
}

bool game_addGameObject(Game_Object* object, int layerID)
{
	if (layerID >= GAME_LAYER_AMOUNT)
	{
		cout << "[WARN] Invalid layer ID provided: " << layerID << endl;
		return false;
	}

	Game_RenderLayer& layer = gameVar_renderLayers[layerID];
	layer.objectList.add(object);
	layer.objectCount++;

	return true;
}

bool game_removeGameObject(Game_Object* object)
{
	bool objectFound = false;

	for (int i = 0; i < GAME_LAYER_AMOUNT; i++)
	{
		Game_RenderLayer& currentLayer = gameVar_renderLayers[i];
		if (currentLayer.objectList.remove(object) && !objectFound)
			objectFound = true;
	}

	return objectFound;
}

Game_Point game_getObjectRenderPos(Game_Object& object)
{
	if (object.isStatic)
	{
		if (object.isModuleEnabled(MODULE_EXTRA_BOUNDS) && object.extraBoundsModule->enabled)
		{
			Game_Rect objectSize = game_getObjectRenderSize(object);
			int targetX, targetY;
			switch (object.extraBoundsModule->floatMode)
			{
				case FLOAT_LEFT_TOP:
					targetX = object.extraBoundsModule->marginLeft + object.extraBoundsModule->paddingLeft;
					targetY = object.extraBoundsModule->marginTop + object.extraBoundsModule->paddingTop;
					break;
				case FLOAT_LEFT_CENTER:
					targetX = object.extraBoundsModule->marginLeft + object.extraBoundsModule->paddingLeft;
					targetY = (gameVar_mainCamera.size.height - objectSize.height) / 2 - object.extraBoundsModule->marginTop;
					break;
				case FLOAT_LEFT_BOTTOM:
					targetX = object.extraBoundsModule->marginLeft + object.extraBoundsModule->paddingLeft;
					targetY = (gameVar_mainCamera.size.height - objectSize.height - object.extraBoundsModule->marginBottom)
							- object.extraBoundsModule->paddingBottom;
					break;
				case FLOAT_CENTER_TOP:
					targetX = (gameVar_mainCamera.size.width - objectSize.width) / 2 - object.extraBoundsModule->marginLeft;
					targetY = object.extraBoundsModule->marginTop + object.extraBoundsModule->paddingTop;
					break;
				case FLOAT_CENTER:
					targetX = (gameVar_mainCamera.size.width - objectSize.width) / 2 - object.extraBoundsModule->marginLeft;
					targetY = (gameVar_mainCamera.size.height - objectSize.height) / 2 - object.extraBoundsModule->marginTop;
					break;
				case FLOAT_CENTER_BOTTOM:
					targetX = (gameVar_mainCamera.size.width - objectSize.width) / 2 - object.extraBoundsModule->marginLeft;
					targetY = (gameVar_mainCamera.size.height - objectSize.height - object.extraBoundsModule->marginBottom)
							- object.extraBoundsModule->paddingBottom;
					break;
				case FLOAT_RIGHT_TOP:
					targetX = (gameVar_mainCamera.size.width - objectSize.width - object.extraBoundsModule->marginRight)
							- object.extraBoundsModule->paddingRight;
					targetY = object.extraBoundsModule->marginTop + object.extraBoundsModule->paddingTop;
					break;
				case FLOAT_RIGHT_CENTER:
					targetX = (gameVar_mainCamera.size.width - objectSize.width - object.extraBoundsModule->marginRight)
							- object.extraBoundsModule->paddingRight;
					targetY = (gameVar_mainCamera.size.height - objectSize.height) / 2 - object.extraBoundsModule->marginTop;
					break;
				case FLOAT_RIGHT_BOTTOM:
					targetX = (gameVar_mainCamera.size.width - objectSize.width - object.extraBoundsModule->marginRight)
							- object.extraBoundsModule->paddingRight;
					targetY = (gameVar_mainCamera.size.height - objectSize.height - object.extraBoundsModule->marginBottom)
							- object.extraBoundsModule->paddingBottom;
					break;
				default:
					targetX = 0;
					targetY = 0;
					break;
			}
			return {targetX, targetY};
		}
		else
			return object.position;
	}

	int targetX, targetY;
	if (!gameVar_keyboardMovesCamera && gameVar_mainCamera.centeredObject)
	{
		int centeredObjectX = (gameVar_mainCamera.size.width - gameVar_mainCamera.centeredObject->size.width) / 2;
		int centeredObjectY = (gameVar_mainCamera.size.height - gameVar_mainCamera.centeredObject->size.height) / 2;
		if (object.getID() == gameVar_mainCamera.centeredObject->getID())
		{
			targetX = centeredObjectX;
			targetY = centeredObjectY;
		}
		else
		{
			targetX = centeredObjectX - gameVar_mainCamera.centeredObject->position.x + object.position.x;
			targetY = centeredObjectY - gameVar_mainCamera.centeredObject->position.y + object.position.y;
		}
	}
	else
	{
		targetX = -gameVar_mainCamera.position.x + object.position.x;
		targetY = -gameVar_mainCamera.position.y + object.position.y;
	}

	return {targetX, targetY};
}

Game_Rect game_getObjectRenderSize(Game_Object& object)
{
	int objectWidth = object.size.width;
	int objectHeight = object.size.height;

	if (object.isModuleEnabled(MODULE_EXTRA_BOUNDS) && object.extraBoundsModule->enabled)
	{
		if (object.extraBoundsModule->fillScreen)
		{
			objectWidth = gameVar_mainCamera.size.width;
			objectHeight = gameVar_mainCamera.size.height;
		}

		objectWidth -= object.extraBoundsModule->paddingLeft;
		objectWidth -= object.extraBoundsModule->paddingRight;
		objectHeight -= object.extraBoundsModule->paddingTop;
		objectHeight -= object.extraBoundsModule->paddingBottom;
	}

	if (!object.isStatic)
	{
		objectWidth *= gameVar_zoomScale;
		objectHeight *= gameVar_zoomScale;
	}

	return {objectWidth, objectHeight};
}

Game_Object* game_findObjectByID(unsigned int objectID)
{
	for (int i = 0; i < GAME_LAYER_AMOUNT; i++)
	{
		Game_Object* foundObject = gameVar_renderLayers[i].objectList.search(objectID);
		if (foundObject)
			return foundObject;
	}

	return NULL;
}

Game_RenderEquipment* game_createRenderEquipment(int surfaceWidth, int surfaceHeight)
{
	// Create surface
	SDL_Surface* surface = SDL_CreateRGBSurface(0, surfaceWidth, surfaceHeight, 32, GAME_SURFACE_RMASK,
	GAME_SURFACE_GMASK,
	GAME_SURFACE_BMASK, GAME_SURFACE_AMASK);

	if (!surface)
	{
		cout << "[ERR] Couldn't create SDL_Surface for render equipment: " << SDL_GetError() << endl;
		return NULL;
	}

	// Create equipment
	SDL_Renderer* renderer = SDL_CreateSoftwareRenderer(surface);
	if (!renderer)
	{
		cout << "[ERR] Couldn't create SDL_Renderer for render equipment: " << SDL_GetError() << endl;
		return NULL;
	}

	return new Game_RenderEquipment(renderer, surface);
}

Game_Rect game_getTextSize(string text, TTF_Font* font)
{
	Game_Rect dest = {-1, -1};
	if (TTF_SizeText(font, text.c_str(), &dest.width, &dest.height))
		cout << "[ERR] Couldn't get size of text '" << text << "'!" << endl;

	return dest;
}

SDL_Surface* colorBackgroundTU(Game_Object& object, Game_RenderEquipment& equipment)
{
	if (!object.isModuleEnabled(MODULE_COLOR_BACKGROUND))
		return NULL;

	SDL_Color bg = object.colorBackgroundModule->backgroundColor;
	SDL_SetRenderDrawColor(equipment.softwareRenderer, bg.r, bg.g, bg.b, bg.a);

	SDL_Rect rect = {0, 0, object.size.width, object.size.height};
	SDL_RenderFillRect(equipment.softwareRenderer, &rect);

	return equipment.surface;
}

SDL_Surface* imageTextureObjectTU(Game_Object& object, Game_RenderEquipment& equipment)
{
	if (!object.isModuleEnabled(MODULE_IMAGE_BACKGROUND))
		return NULL;

	string texturePath = object.imageBackgroundModule->getTexturePath();
	SDL_Surface* loadedSurface = game_getAsset(texturePath);
	if (loadedSurface)
		return loadedSurface;

	if (!game_loadAsset(texturePath))
	{
		cout << "[ERROR] Couldn't load file '" << texturePath << "': " << IMG_GetError() << endl;
		return NULL;
	}

	return game_getAsset(texturePath);
}

SDL_Surface* textObjectTU(Game_Object& object, Game_RenderEquipment& equipment)
{
	if (!object.isModuleEnabled(MODULE_TEXT))
		return NULL;

	SDL_Surface* textSurface = object.textModule->renderText();
	if (!textSurface)
		return NULL;
	else if (object.textModule->autoSize)
	{
		object.size.width = textSurface->w * object.textModule->textScaling;
		object.size.height = textSurface->h * object.textModule->textScaling;
	}

	return textSurface;
}
