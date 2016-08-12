#include <iostream>
#include "shared.h"

/* Shared Memory */

bool Game_SharedMemory::p_running = false;
int Game_SharedMemory::p_targetFPS = 60;
bool Game_SharedMemory::p_useFPSCounter = true;

Game_Camera Game_SharedMemory::w_mainCamera = { {0, 0}, {1024, 576}, 0, 2};
bool Game_SharedMemory::w_keyboardMovesCamera = true;
double Game_SharedMemory::w_zoomScale = 1.0;

Game_RenderLayer* Game_SharedMemory::r_renderLayers = new Game_RenderLayer[GAME_LAYER_AMOUNT];
int Game_SharedMemory::r_renderThreadID = -1;

bool Game_SharedMemory::s_SDLInitialized = false;
SDL_Renderer* Game_SharedMemory::s_mainRenderer = NULL;
SDL_Window* Game_SharedMemory::s_window = NULL;

string Game_SharedMemory::m_assetsFolder = "assets";
Game_AdvancedObject* Game_SharedMemory::m_keyboardInputObject = NULL;	// TODO: Replace input object by input handler
Game_AdvancedObject* Game_SharedMemory::m_fpsObject = NULL;
TTF_Font* Game_SharedMemory::m_guiFont = NULL;

bool Game_Tools::addGameObject(Game_Object* object, unsigned int layerID)
{
	if (layerID >= GAME_LAYER_AMOUNT)
	{
		cout << "[WARN] Invalid layer ID provided: " << layerID << endl;
		return false;
	}

	Game_RenderLayer* layer = &Game_SharedMemory::r_renderLayers[layerID];
	Game_ObjectNode* newObjectNode = new Game_ObjectNode();
	newObjectNode->object = object;
	newObjectNode->nextNode = layer->objectList;

	layer->objectList = newObjectNode;
	layer->objectCount++;

	return true;
}

bool Game_Tools::removeGameObject(Game_Object* object)
{
	for (int i = 0; i < GAME_LAYER_AMOUNT; i++)
	{
		Game_RenderLayer* currentLayer = &Game_SharedMemory::r_renderLayers[i];
		Game_ObjectNode* currentObjectNode = currentLayer->objectList;
		while (currentObjectNode != NULL)
		{
			if (currentObjectNode->object->getID() == object->getID())
			{
				if (currentObjectNode->prevNode != NULL)
					currentObjectNode->prevNode->nextNode = currentObjectNode->nextNode;

				if (currentObjectNode->nextNode != NULL)
					currentObjectNode->nextNode->prevNode = currentObjectNode->prevNode;

				currentObjectNode->prevNode = NULL;
				currentObjectNode->nextNode = NULL;
				delete currentObjectNode;
				currentLayer->objectCount--;

				return true;
			}

			currentObjectNode = currentObjectNode->nextNode;
		}
	}

	return false;
}

Game_Rect Game_Tools::getTextSize(string text)
{
	TTF_Font* font = Game_SharedMemory::m_guiFont;
	Game_Rect dest = {0, 0};

	if (TTF_SizeText(font, text.c_str(), &dest.width, &dest.height))
		cout << "[ERR] Couldn't get size of text '" << text << "'!" << endl;

	return dest;
}

Game_RenderEquipment* Game_Tools::createRenderEquipment(int surfaceWidth, int surfaceHeight)
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
