#include <iostream>
#include <SDL2/SDL_image.h>

#include "game_defs.h"
#include "game_shm.h"
#include "game_tools.h"
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
		xCheck = (childPos.x < parentPos.x && (childPos.x + childSize.width) >= parentPos.x &&
				(childPos.x + childSize.width) <= (parentPos.x + parentSize.width));
	}
	if (!yCheck)
	{
		yCheck = (childPos.y < parentPos.y && (childPos.y + childSize.height) >= parentPos.y &&
				(childPos.y + childSize.height) <= (parentPos.y + parentSize.height));
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
		return false;

	LinkedListNode<Game_Asset>* newNode = new LinkedListNode<Game_Asset>();
	newNode->value = new Game_Asset(assetPath, loadedSurface);
	newNode->nextNode = game_shmGet(SHM_ASSETS_LOADED_ASSETS);
	game_shmPut(SHM_ASSETS_LOADED_ASSETS, newNode);
	return true;
}

SDL_Surface* game_getAsset(string assetPath)
{
	LinkedListNode<Game_Asset>* currentNode = game_shmGet(SHM_ASSETS_LOADED_ASSETS);
	while (currentNode)
	{
		if (currentNode->value->assetPath == assetPath)
			return currentNode->value->loadedSurface;

		currentNode = currentNode->nextNode;
	}

	return NULL;
}

void game_freeAssets()
{
	GAME_DEBUG_CHECK
		cout << "[DEBUG] Freeing assets... ";

	if (game_shmGet(SHM_ASSETS_LOADED_ASSETS))
		delete game_shmGet(SHM_ASSETS_LOADED_ASSETS);

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

	Game_RenderLayer* layer = &game_shmGet(SHM_RENDER_LAYERS)[layerID];
	LinkedListNode<Game_Object>* newNode = new LinkedListNode<Game_Object>();
	newNode->value = object;
	newNode->nextNode = layer->objectList;

	layer->objectList = newNode;
	layer->objectCount++;

	return true;
}

bool game_removeGameObject(Game_Object* object)
{
	Game_RenderLayer* renderLayer = NULL;
	LinkedListNode<Game_Object>* targetNode = NULL;
	game_findObjectByID(object->getID(), &renderLayer, &targetNode);

	if (targetNode && renderLayer)
	{
		if (targetNode->prevNode)
			targetNode->prevNode->nextNode = targetNode->nextNode;

		if (targetNode->nextNode)
			targetNode->nextNode->prevNode = targetNode->prevNode;

		delete targetNode;
		renderLayer->objectCount--;

		return true;
	}

	return false;
}

Game_Point game_getObjectRenderPos(Game_Object& object)
{
	// RXro = Render X random object
	// RYro = Render Y random object

	Game_Camera& mainCamera = game_shmGet(SHM_WORLD_MAIN_CAMERA);
	if (object.isStatic)
	{
		if (object.isModuleEnabled(MODULE_MARGIN) && object.marginModule->enabled)
		{
			Game_Rect objectSize = game_getObjectRenderSize(object);
			int RXro, RYro;
			switch (object.marginModule->floatMode)
			{
				case FLOAT_LEFT_TOP:
					RXro = object.marginModule->marginLeft;
					RYro = object.marginModule->marginTop;
					break;
				case FLOAT_LEFT_CENTER:
					RXro = object.marginModule->marginLeft;
					RYro = (mainCamera.size.height - objectSize.height) / 2 - object.marginModule->marginTop;
					break;
				case FLOAT_LEFT_BOTTOM:
					RXro = object.marginModule->marginLeft;
					RYro = (mainCamera.size.height - objectSize.height - object.marginModule->marginBottom);
					break;
				case FLOAT_CENTER_TOP:
					RXro = (mainCamera.size.width - objectSize.width) / 2 - object.marginModule->marginLeft;
					RYro = object.marginModule->marginTop;
					break;
				case FLOAT_CENTER:
					RXro = (mainCamera.size.width - objectSize.width) / 2 - object.marginModule->marginLeft;
					RYro = (mainCamera.size.height - objectSize.height) / 2 - object.marginModule->marginTop;
					break;
				case FLOAT_CENTER_BOTTOM:
					RXro = (mainCamera.size.width - objectSize.width) / 2 - object.marginModule->marginLeft;
					RYro = (mainCamera.size.height - objectSize.height - object.marginModule->marginBottom);
					break;
				case FLOAT_RIGHT_TOP:
					RXro = (mainCamera.size.width - objectSize.width - object.marginModule->marginRight);
					RYro = object.marginModule->marginTop;
					break;
				case FLOAT_RIGHT_CENTER:
					RXro = (mainCamera.size.width - objectSize.width - object.marginModule->marginRight);
					RYro = (mainCamera.size.height - objectSize.height) / 2 - object.marginModule->marginTop;
					break;
				case FLOAT_RIGHT_BOTTOM:
					RXro = (mainCamera.size.width - objectSize.width - object.marginModule->marginRight);
					RYro = (mainCamera.size.height - objectSize.height - object.marginModule->marginBottom);
					break;
				default:
					RXro = 0;
					RYro = 0;
					break;
			}
			return {RXro, RYro};
		}
		else
			return object.position;
	}

	int RXro, RYro;
	if (!game_shmGet(SHM_WORLD_KEYBOARD_MOVES_CAMERA) && mainCamera.centeredObject)
	{
		// RXco = Render X centered object
		// RYco = Render Y centered object
		int RXco = (mainCamera.size.width - mainCamera.centeredObject->size.width) / 2;
		int RYco = (mainCamera.size.height - mainCamera.centeredObject->size.height) / 2;
		if (object.getID() == mainCamera.centeredObject->getID())
		{
			RXro = RXco;
			RYro = RYco;
		}
		else
		{
			RXro = RXco - mainCamera.centeredObject->position.x + object.position.x;
			RYro = RYco - mainCamera.centeredObject->position.y + object.position.y;
		}
	}
	else
	{
		RXro = -mainCamera.position.x + object.position.x;
		RYro = -mainCamera.position.y + object.position.y;
	}

	return {RXro, RYro};
}

Game_Rect game_getObjectRenderSize(Game_Object& object)
{
	Game_Camera& mainCamera = game_shmGet(SHM_WORLD_MAIN_CAMERA);
	int width = object.size.width;
	int height = object.size.height;

	if (width < 0)
		width += (mainCamera.size.width + 1);
	if (height < 0)
		height += (mainCamera.size.height + 1);

	if (!object.isStatic)
	{
		width *= game_shmGet(SHM_WORLD_ZOOM_SCALE);
		height *= game_shmGet(SHM_WORLD_ZOOM_SCALE);
	}

	return {width, height};
}

Game_Object* game_findObjectByID(unsigned int objectID, Game_RenderLayer** outputLayer, LinkedListNode<Game_Object>** outputNode)
{
	for (int i = 0; i < GAME_LAYER_AMOUNT; i++)
	{
		Game_RenderLayer& currentLayer = game_shmGet(SHM_RENDER_LAYERS)[i];
		LinkedListNode<Game_Object>* currentNode = currentLayer.objectList;
		while (currentNode)
		{
			if (currentNode->value->getID() == objectID)
			{
				if (outputLayer)
					*outputLayer = &currentLayer;

				if (outputNode)
					*outputNode = currentNode;

				return currentNode->value;
			}

			currentNode = currentNode->nextNode;
		}
	}

	return NULL;
}

Game_RenderEquipment* game_createRenderEquipment(int surfaceWidth, int surfaceHeight)
{
	// Create surface
	SDL_Surface* surface = SDL_CreateRGBSurface(0, surfaceWidth, surfaceHeight, 32, GAME_SURFACE_RMASK, GAME_SURFACE_GMASK,
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

SDL_Surface* imageTextureObjectTU(Game_Object& object, Game_RenderEquipment* equipment)
{
	if (!object.isModuleEnabled(MODULE_IMAGE_BACKGROUND))
		return NULL;

	string texturePath = object.imageBackgroundModule->getTexturePath();
	SDL_Surface* loadedSurface = game_getAsset(texturePath);
	if (loadedSurface)
		return loadedSurface;

	if (!game_loadAsset(texturePath))
	{
		cout << "[ERR] Couldn't load file '" << texturePath << "': " << IMG_GetError() << endl;
		return NULL;
	}

	return game_getAsset(texturePath);
}

SDL_Surface* textObjectTextureUpdate(Game_Object& object, Game_RenderEquipment* equipment)
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
