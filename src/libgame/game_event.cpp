#include <iostream>
#include "game_defs.h"
#include "game_event.h"
#include "game_tools.h"
#include "game_shm.h"

using namespace std;

bool processMovement(SDL_KeyboardEvent& event)
{
	Game_Camera* mainCamera = &game_shmGet(SHM_WORLD_MAIN_CAMERA);
	switch (event.keysym.scancode)
	{
		case SDL_SCANCODE_W:
			if (event.state == SDL_PRESSED)
			{
				mainCamera->movementDirection &= 0xD;   // Remove 'backward' movement
				mainCamera->movementDirection |= 0x1;   // Add 'forward' movement
			}
			else
				mainCamera->movementDirection &= 0xE;   // Remove 'forward' movement

			break;
		case SDL_SCANCODE_A:
			if (event.state == SDL_PRESSED)
			{
				mainCamera->movementDirection &= 0x7;   // Remove 'right' movement
				mainCamera->movementDirection |= 0x4;   // Add 'left' movement
			}
			else
				mainCamera->movementDirection &= 0xB;   // Remove 'left' movement

			break;
		case SDL_SCANCODE_S:
			if (event.state == SDL_PRESSED)
			{
				mainCamera->movementDirection &= 0xE;   // Remove 'forward' movement
				mainCamera->movementDirection |= 0x2;   // Add 'backward' movement
			}
			else
				mainCamera->movementDirection &= 0xD;   // Remove 'backward' movement

			break;
		case SDL_SCANCODE_D:
			if (event.state == SDL_PRESSED)
			{
				mainCamera->movementDirection &= 0xB;   // Remove 'left' movement
				mainCamera->movementDirection |= 0x8;   // Add 'right' movement
			}
			else
				mainCamera->movementDirection &= 0x7;   // Remove 'right' movement

			break;
		default:
			break;
	}

	return true;
}

bool processRepeatKeys(SDL_KeyboardEvent& event)
{
	// Check for other recognized keys
	if (event.state == SDL_PRESSED)
	{
		bool keyProcessed = true;
		switch (event.keysym.scancode)
		{
			case SDL_SCANCODE_UP:
				game_shmGet(SHM_WORLD_ZOOM_SCALE) += 0.1f;
				break;
			case SDL_SCANCODE_DOWN:
				if (game_shmGet(SHM_WORLD_ZOOM_SCALE))
					game_shmGet(SHM_WORLD_ZOOM_SCALE) -= 0.1f;
				break;
			case SDL_SCANCODE_SPACE:
			{
				Game_Camera* mainCamera = &game_shmGet(SHM_WORLD_MAIN_CAMERA);
				if (event.keysym.mod & KMOD_LSHIFT)
				{
					if (mainCamera->movementSpeed > 1)
						mainCamera->movementSpeed -= 1;
				}
				else
					mainCamera->movementSpeed += 1;

				break;
			}
			default:
				keyProcessed = false;
				break;
		}

		return keyProcessed;
	}

	return false;
}

bool processNonRepeatKeys(SDL_KeyboardEvent& event)
{
	// Check for movement keys
	bool keyProcessed = false;
	switch (event.keysym.scancode)
	{
		case SDL_SCANCODE_W:
		case SDL_SCANCODE_A:
		case SDL_SCANCODE_S:
		case SDL_SCANCODE_D:
			keyProcessed = processMovement(event);
			break;
		default:
			break;
	}

	if (keyProcessed)
		return true;

	// Check for other keys
	if (event.state == SDL_PRESSED)
	{
		keyProcessed = true;
		switch (event.keysym.scancode)
		{
			case SDL_SCANCODE_R:
			{
				Game_Camera* mainCamera = &game_shmGet(SHM_WORLD_MAIN_CAMERA);
				if (!game_shmGet(SHM_WORLD_KEYBOARD_MOVES_CAMERA) && mainCamera->centeredObject)
				{
					mainCamera->centeredObject->position.x = 0;
					mainCamera->centeredObject->position.y = 0;
					mainCamera->centeredObject->rotation = 0;
				}

				mainCamera->movementSpeed = 2;
				game_shmGet(SHM_WORLD_ZOOM_SCALE) = 1.0;
				break;
			}
			case SDL_SCANCODE_F11:
				if (event.type == SDL_KEYDOWN)
				{
					static int fullScreen = 0;
					fullScreen = (!fullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

					SDL_SetWindowFullscreen(game_shmGet(SHM_SDL_WINDOW), fullScreen);
				}
				break;
			default:
				keyProcessed = false;
		}
	}

	return keyProcessed;
}

void game_processKeyboardEvent(SDL_Event& event)
{
	// Forward event to correct function
	if (!event.key.repeat)
	{
		if (processNonRepeatKeys(event.key))
			return;
	}

	if (processRepeatKeys(event.key))
		return;

	// Forward event to input object
	if (game_shmGet(SHM_MISC_KEYBOARD_INPUT_OBJECT)
	        && game_shmGet(SHM_MISC_KEYBOARD_INPUT_OBJECT)->isModuleEnabled(MODULE_EVENT))
	{
		Game_ObjectEvent objectEvent(&event);
		game_shmGet(SHM_MISC_KEYBOARD_INPUT_OBJECT)->eventModule->callEventFunction(EVENT_TYPE_TYPED, objectEvent);
	}
}

void game_processMouseEvent(SDL_Event& event)
{
	// Find object which was clicked
	int clickedX = event.button.x;
	int clickedY = event.button.y;
	for (int i = 0; i < GAME_LAYER_AMOUNT; i++)
	{
		LinkedListNode<Game_Object>* currentNode = game_shmGet(SHM_RENDER_LAYERS)[i].objectList;
		while (currentNode)
		{
			Game_Object* object = currentNode->value;
			if (object->isModuleEnabled(MODULE_EVENT))
			{
				Game_Point renderPos = game_getObjectRenderPos(*object);
				Game_Rect renderSize = game_getObjectRenderSize(*object);
				if (game_isInside(renderPos, renderSize, {clickedX, clickedY}, {0, 0}, true))
				{
					// Create Game_MouseClickedEvent
					Game_MouseClickedEvent mouseClickedEvent(&event);
					mouseClickedEvent.relX = clickedX - renderPos.x;
					mouseClickedEvent.relY = clickedY - renderPos.y;

					object->eventModule->callEventFunction(EVENT_TYPE_CLICKED, mouseClickedEvent);
					return;
				}
			}

			currentNode = currentNode->nextNode;
		}
	}
}

void game_processWindowEvent(SDL_Event& event)
{
	switch (event.window.event)
	{
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			int w, h;
			SDL_GetWindowSize(game_shmGet(SHM_SDL_WINDOW), &w, &h);

			game_shmGet(SHM_WORLD_MAIN_CAMERA).size.width = w;
			game_shmGet(SHM_WORLD_MAIN_CAMERA).size.height = h;

			SDL_RenderClear(game_shmGet(SHM_SDL_MAIN_RENDERER));
			break;
		case SDL_WINDOWEVENT_CLOSE:
			game_shmPut(SHM_GAME_IS_RUNNING, false);
			break;
	}
}
