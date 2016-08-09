#include <iostream>
#include "event.h"

using namespace std;

bool processCameraMovement(SDL_KeyboardEvent& event)
{
	if (!Game_SharedMemory::w_keyboardMovesCamera)
		return false;

	Game_Camera& mainCamera = Game_SharedMemory::w_mainCamera;
	switch (event.keysym.scancode)
	{
		case SDL_SCANCODE_W:
			if (event.state == SDL_PRESSED)
			{
				mainCamera.m_movementDirection &= 0xD;   // Remove 'backward' movement
				mainCamera.m_movementDirection |= 0x1;   // Add 'forward' movement
			}
			else
			{
				mainCamera.m_movementDirection &= 0xE;   // Remove 'forward' movement
			}
			break;
		case SDL_SCANCODE_A:
			if (event.state == SDL_PRESSED)
			{
				mainCamera.m_movementDirection &= 0x7;   // Remove 'right' movement
				mainCamera.m_movementDirection |= 0x4;   // Add 'left' movement
			}
			else
			{
				mainCamera.m_movementDirection &= 0xB;   // Remove 'left' movement
			}
			break;
		case SDL_SCANCODE_S:
			if (event.state == SDL_PRESSED)
			{
				mainCamera.m_movementDirection &= 0xE;   // Remove 'forward' movement
				mainCamera.m_movementDirection |= 0x2;   // Add 'backward' movement
			}
			else
			{
				mainCamera.m_movementDirection &= 0xD;   // Remove 'backward' movement
			}
			break;
		case SDL_SCANCODE_D:
			if (event.state == SDL_PRESSED)
			{
				mainCamera.m_movementDirection &= 0xB;   // Remove 'left' movement
				mainCamera.m_movementDirection |= 0x8;   // Add 'right' movement
			}
			else
			{
				mainCamera.m_movementDirection &= 0x7;   // Remove 'right' movement
			}
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
				Game_SharedMemory::w_zoomScale += 0.1f;
				break;
			case SDL_SCANCODE_DOWN:
				if (Game_SharedMemory::w_zoomScale)
					Game_SharedMemory::w_zoomScale -= 0.1f;
				break;
			case SDL_SCANCODE_SPACE:
				if (event.keysym.mod & KMOD_LSHIFT)
				{
					if (Game_SharedMemory::w_mainCamera.m_movementSpeed > 1.0)
						Game_SharedMemory::w_mainCamera.m_movementSpeed -= 1.0;
				}
				else
					Game_SharedMemory::w_mainCamera.m_movementSpeed += 1.0;
				break;
			case SDL_SCANCODE_F11:
				if (event.type == SDL_KEYDOWN)
				{
					static int fullScreen = 0;
					fullScreen = (!fullScreen ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);

					SDL_SetWindowFullscreen(Game_SharedMemory::s_window, fullScreen);
				}
				break;
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
			keyProcessed = processCameraMovement(event);
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
				Game_SharedMemory::w_mainCamera.m_position.x = 0;
				Game_SharedMemory::w_mainCamera.m_position.y = 0;
				Game_SharedMemory::w_mainCamera.m_movementSpeed = 2;
				Game_SharedMemory::w_zoomScale = 1;
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
	if (Game_SharedMemory::m_keyboardInputObject)
	{
		Game_SharedMemory::m_keyboardInputObject->callEventFunction(EVENT_TYPE_KEY, event);
	}
}

void game_processMouseEvent(SDL_Event& event)
{

}

void game_processWindowEvent(SDL_Event& event)
{
	switch (event.window.event)
	{
		case SDL_WINDOWEVENT_SIZE_CHANGED:
			int w, h;
			SDL_GetWindowSize(Game_SharedMemory::s_window, &w, &h);

			Game_SharedMemory::w_mainCamera.m_size.width = w;
			Game_SharedMemory::w_mainCamera.m_size.height = h;

			SDL_RenderClear(Game_SharedMemory::s_mainRenderer);
			break;
		case SDL_WINDOWEVENT_CLOSE:
			Game_SharedMemory::p_running = false;
			break;
	}
}
