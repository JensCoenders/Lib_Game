#ifndef GAME_SHARED_H
#define GAME_SHARED_H

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <string>
#include "object.h"
#include "types.h"

using namespace std;

/* Shared Memory */

class Game_SharedMemory
{
	public:
		// Properties (p)
		static bool p_running;
		static int p_targetFPS;
		static bool p_useFPSCounter;

		// World (w)
		static Game_Camera w_mainCamera;
		static bool w_keyboardMovesCamera;
		static float w_zoomScale;

		// Rendering (r)
		static Game_RenderLayer* r_renderLayers;
		static int r_renderThreadID;

		// SDL (s)
		static bool s_SDLInitialized;
		static SDL_Renderer* s_mainRenderer;
		static SDL_Window* s_window;

		// Miscellaneous (m)
		static Game_Object* m_keyboardInputObject;
		static Game_GUIObject* m_fpsObject;
		static TTF_Font* m_guiFont;

		// Functions
		static bool addGameObject(Game_Object* object, unsigned int layerID);
		static bool removeGameObject(Game_Object* object);

};

#endif
