#include <iostream>
#include "game_defs.h"
#include "game_event.h"

#include "game_global.h"
#include "game_tools.h"

using namespace std;

bool processMovement(SDL_KeyboardEvent& event)
{
	switch (event.keysym.scancode)
	{
		case SDL_SCANCODE_W:
			if (event.state == SDL_PRESSED)
			{
				gameVar_mainCamera.movementDirection &= 0xD;   // Remove 'backward' movement
				gameVar_mainCamera.movementDirection |= 0x1;   // Add 'forward' movement
			}
			else
				gameVar_mainCamera.movementDirection &= 0xE;   // Remove 'forward' movement

			break;
		case SDL_SCANCODE_A:
			if (event.state == SDL_PRESSED)
			{
				gameVar_mainCamera.movementDirection &= 0x7;   // Remove 'right' movement
				gameVar_mainCamera.movementDirection |= 0x4;   // Add 'left' movement
			}
			else
				gameVar_mainCamera.movementDirection &= 0xB;   // Remove 'left' movement

			break;
		case SDL_SCANCODE_S:
			if (event.state == SDL_PRESSED)
			{
				gameVar_mainCamera.movementDirection &= 0xE;   // Remove 'forward' movement
				gameVar_mainCamera.movementDirection |= 0x2;   // Add 'backward' movement
			}
			else
				gameVar_mainCamera.movementDirection &= 0xD;   // Remove 'backward' movement

			break;
		case SDL_SCANCODE_D:
			if (event.state == SDL_PRESSED)
			{
				gameVar_mainCamera.movementDirection &= 0xB;   // Remove 'left' movement
				gameVar_mainCamera.movementDirection |= 0x8;   // Add 'right' movement
			}
			else
				gameVar_mainCamera.movementDirection &= 0x7;   // Remove 'right' movement

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
				gameVar_zoomScale += 0.1;
				break;
			case SDL_SCANCODE_DOWN:
				if (gameVar_zoomScale > 0.2)
					gameVar_zoomScale -= 0.1;

				break;
			case SDL_SCANCODE_SPACE:
			{
				if (event.keysym.mod & KMOD_LSHIFT)
				{
					if (gameVar_mainCamera.movementSpeed > 1)
						gameVar_mainCamera.movementSpeed -= 1;
				}
				else
					gameVar_mainCamera.movementSpeed += 1;

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

	if (event.state == SDL_PRESSED)
	{
		keyProcessed = true;
		switch (event.keysym.scancode)
		{
			case SDL_SCANCODE_Q:
				gameVar_keyboardMovesCamera = !gameVar_keyboardMovesCamera;
				break;
			case SDL_SCANCODE_R:
			{
				if (gameVar_keyboardMovesCamera)
				{
					gameVar_mainCamera.position.x = 0;
					gameVar_mainCamera.position.y = 0;
				}
				else if (gameVar_mainCamera.centeredObject)
				{
					gameVar_mainCamera.centeredObject->position.x = 0;
					gameVar_mainCamera.centeredObject->position.y = 0;
					gameVar_mainCamera.centeredObject->rotation = 0;
				}

				gameVar_mainCamera.movementSpeed = 2;
				gameVar_zoomScale = 1.0;
				break;
			}
			case SDL_SCANCODE_F11:
				if (event.type == SDL_KEYDOWN)
				{
					static int fullScreen = 0;
					fullScreen = (!fullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

					SDL_SetWindowFullscreen(gameVar_mainWindow, fullScreen);
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
	if (!event.key.repeat && processNonRepeatKeys(event.key))
		return;
	else if (processRepeatKeys(event.key))
		return;
	else if (gameVar_keyboardInputObject && gameVar_keyboardInputObject->isModuleEnabled(MODULE_EVENT))
	{
		Game_KeyTypedEvent keyTypedEvent(&event);
		gameVar_keyboardInputObject->eventModule->callEventFunction(EVENT_TYPE_TYPED, keyTypedEvent);
	}
}

void game_processMouseEvent(SDL_Event& event)
{
	for (int i = 0; i < GAME_LAYER_AMOUNT; i++)
	{
		LinkedListNode<Game_Object>* currentNode = gameVar_renderLayers[i].objectList.nodes;
		while (currentNode)
		{
			Game_Object& currentObject = *currentNode->value;
			if (!currentObject.isModuleEnabled(MODULE_EVENT))
			{
				currentNode = currentNode->nextNode;
				continue;
			}

			Game_Point renderPos = game_getObjectRenderPos(currentObject);
			Game_Rect renderSize = game_getObjectRenderSize(currentObject);
			switch (event.type)
			{
				case SDL_MOUSEBUTTONUP:
				case SDL_MOUSEBUTTONDOWN:
				{
					int clickedX = event.button.x;
					int clickedY = event.button.y;
					if (game_isInside(renderPos, renderSize, {clickedX, clickedY}, {0, 0}, true))
					{
						Game_MouseClickedEvent mouseClickedEvent(&event);
						mouseClickedEvent.relativeX = clickedX - renderPos.x;
						mouseClickedEvent.relativeY = clickedY - renderPos.y;

						currentObject.eventModule->callEventFunction(EVENT_TYPE_CLICKED, mouseClickedEvent);
					}
					break;
				}
				case SDL_MOUSEMOTION:
				{
					int currentX = event.motion.x;
					int currentY = event.motion.y;
					Game_MouseHoveredEvent mouseMotionEvent(&event);
					if (game_isInside(renderPos, renderSize, {currentX, currentY}, {0, 0}, true))
					{
						if (!currentObject.eventModule->mouseHovering)
						{
							mouseMotionEvent.entering = true;
							currentObject.eventModule->mouseHovering = true;
							currentObject.eventModule->callEventFunction(EVENT_TYPE_MOTION, mouseMotionEvent);
						}
					}
					else
					{
						if (currentObject.eventModule->mouseHovering)
						{
							mouseMotionEvent.entering = false;
							currentObject.eventModule->mouseHovering = false;
							currentObject.eventModule->callEventFunction(EVENT_TYPE_MOTION, mouseMotionEvent);
						}
					}
					break;
				}
				default:
					break;
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
			SDL_GetWindowSize(gameVar_mainWindow, &w, &h);

			gameVar_mainCamera.size.width = w;
			gameVar_mainCamera.size.height = h;

			SDL_RenderClear(gameVar_mainRenderer);
			break;
		case SDL_WINDOWEVENT_CLOSE:
			gameVar_isRunning = false;
			break;
	}
}

Game_ObjectEvent::Game_ObjectEvent(SDL_Event* event)
{
	this->originalEvent = event;
}

Game_KeyTypedEvent::Game_KeyTypedEvent(SDL_Event* event) :
		Game_ObjectEvent(event)
{
	this->scancode = event->key.keysym.scancode;
	this->pressed = event->key.state == SDL_PRESSED;
	this->keyMod = event->key.keysym.mod;
	this->repeat = event->key.repeat;
}

Game_MouseClickedEvent::Game_MouseClickedEvent(SDL_Event* event) :
		Game_ObjectEvent(event)
{
	this->button = event->button.button;
	this->pressed = (event->button.state == SDL_PRESSED);
	this->relativeX = 0;
	this->relativeY = 0;
}

Game_MouseHoveredEvent::Game_MouseHoveredEvent(SDL_Event* event) :
		Game_ObjectEvent(event)
{
	this->entering = false;
	this->newX = event->motion.x;
	this->newY = event->motion.y;
	this->movedX = 0;
	this->movedY = 0;
}
