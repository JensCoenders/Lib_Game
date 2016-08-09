#include <iostream>
#include "shared.h"

/* Shared Memory */

bool Game_SharedMemory::p_running = false;
int Game_SharedMemory::p_targetFPS = 60;
bool Game_SharedMemory::p_useFPSCounter = true;

Game_Camera Game_SharedMemory::w_mainCamera = {{0, 0}, {1024, 576}, 0, 2};
bool Game_SharedMemory::w_keyboardMovesCamera = true;
float Game_SharedMemory::w_zoomScale = 1.0f;

Game_RenderLayer* Game_SharedMemory::r_renderLayers = new Game_RenderLayer[GAME_LAYER_AMOUNT];
int Game_SharedMemory::r_renderThreadID = -1;

bool Game_SharedMemory::s_SDLInitialized = false;
SDL_Renderer* Game_SharedMemory::s_mainRenderer = NULL;
SDL_Window* Game_SharedMemory::s_window = NULL;

Game_Object* Game_SharedMemory::m_keyboardInputObject = NULL;	// TODO: Replace input object by input handler
Game_GUIObject* Game_SharedMemory::m_fpsObject = NULL;
TTF_Font* Game_SharedMemory::m_guiFont = NULL;

bool Game_SharedMemory::addGameObject(Game_Object* object, unsigned int layerID)
{
	if (layerID >= GAME_LAYER_AMOUNT)
	{
		cout << "[WARN] Invalid layer ID provided: " << layerID << endl;
		return false;
	}

	Game_RenderLayer* layer = &r_renderLayers[layerID];
	Game_ObjectNode* newObjectNode = new Game_ObjectNode();
	newObjectNode->object = object;
	newObjectNode->nextNode = layer->objectList;

	layer->objectList = newObjectNode;
	layer->objectCount++;

	return true;
}

bool Game_SharedMemory::removeGameObject(Game_Object* object)
{
	for (int i = 0; i < GAME_LAYER_AMOUNT; i++)
	{
		Game_RenderLayer* currentLayer = &r_renderLayers[i];
		Game_ObjectNode* currentObjectNode = currentLayer->objectList;
		while (currentObjectNode != NULL)
		{
			if (currentObjectNode->object->getID() == object->getID())
			{
				if (currentObjectNode->prevNode != NULL)
				{
					currentObjectNode->prevNode->nextNode = currentObjectNode->nextNode;
				}

				if (currentObjectNode->nextNode != NULL)
				{
					currentObjectNode->nextNode->prevNode = currentObjectNode->prevNode;
				}

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
