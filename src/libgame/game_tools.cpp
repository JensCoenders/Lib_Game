#include <iostream>
#include <SDL2/SDL_image.h>
#include "game_defs.h"
#include "game_tools.h"

using namespace std;

bool game_renderAddObject(Game_Object* object, int layerID)
{
	if (layerID >= GAME_LAYER_AMOUNT)
	{
		cout << "[WARN] Invalid layer ID provided: " << layerID << endl;
		return false;
	}

	Game_RenderLayer& layer = game_shmGet(SHM_RENDER_LAYERS)[layerID];
	LinkedListNode<Game_Object>* newNode = new LinkedListNode<Game_Object>();
	newNode->value = object;
	newNode->nextNode = layer.objectList;

	layer.objectList = newNode;
	layer.objectCount++;

	return true;
}

bool game_renderRemoveObject(Game_Object* object)
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

Game_Rect game_renderGetTextSize(string text)
{
	TTF_Font* font = game_shmGet(SHM_MISC_GUI_FONT);
	Game_Rect dest = {0, 0};

	if (TTF_SizeText(font, text.c_str(), &dest.width, &dest.height))
		cout << "[ERR] Couldn't get size of text '" << text << "'!" << endl;

	return dest;
}

Game_RenderEquipment* game_renderCreateEquipment(int surfaceWidth, int surfaceHeight)
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

Game_Object* game_findObjectByID(int objectID, Game_RenderLayer** outputLayer, LinkedListNode<Game_Object>** outputNode)
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

SDL_Surface* imageTextureObjectTU(Game_Object& object, Game_RenderEquipment* equipment)
{
	Game_RP_ImageTexture* renderPars = dynamic_cast<Game_RP_ImageTexture*>(object.getRenderPars());
	if (!renderPars)
		return NULL;

	SDL_Surface* imageTextureSurface = IMG_Load(renderPars->imageTexturePath.c_str());

	if (!imageTextureSurface)
	{
		cout << "[ERR]: " << IMG_GetError() << endl;
		return NULL;
	}

	return imageTextureSurface;
}
