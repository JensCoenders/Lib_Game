#include <iostream>
#include "shared.h"

/* Shared Memory */

bool Game_SharedMemory::p_running = false;
int Game_SharedMemory::p_targetFPS = 60;
bool Game_SharedMemory::p_useFPSCounter = true;
float Game_SharedMemory::p_zoomScale = 1.0f;

Game_Point Game_SharedMemory::r_cameraCoords = {0, 0};
Game_Rect Game_SharedMemory::r_cameraSize = {0, 0};
Game_Layer* Game_SharedMemory::r_layers = new Game_Layer[GAME_LAYER_AMOUNT];
int Game_SharedMemory::r_renderThreadID = -1;

bool Game_SharedMemory::s_SDLInitialized = false;
SDL_Renderer* Game_SharedMemory::s_mainRenderer = NULL;
SDL_Window* Game_SharedMemory::s_window = NULL;

Game_Object* Game_SharedMemory::m_keyboardInputObject = NULL;
Game_GUIObject* Game_SharedMemory::m_fpsObject = NULL;
TTF_Font* Game_SharedMemory::m_guiFont = NULL;


bool Game_SharedMemory::startRenderingObject(Game_Object* object, unsigned int layerID)
{
	if (layerID >= GAME_LAYER_AMOUNT)
	{
		cout << "[WARN] Invalid layer ID provided: " << layerID << endl;
		return false;
	}

	Game_Layer* layer = &r_layers[layerID];
	Game_ObjectNode* newObjectNode = new Game_ObjectNode();
	newObjectNode->object = object;
	newObjectNode->nextNode = layer->objectList;

	layer->objectList = newObjectNode;
	layer->objectCount++;

	return true;
}

bool Game_SharedMemory::stopRenderingObject(Game_Object* object)
{
	for (int i = 0; i < GAME_LAYER_AMOUNT; i++)
	{
		Game_Layer* currentLayer = &r_layers[i];
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
		}
	}

	return false;
}
