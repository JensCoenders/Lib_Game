#ifndef GAME_SHARED_H
#define GAME_SHARED_H

#include <SDL.h>
#include <SDL_ttf.h>

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
		static double w_zoomScale;

		// Rendering (r)
		static Game_RenderLayer* r_renderLayers;
		static int r_renderThreadID;

		// SDL (s)
		static bool s_SDLInitialized;
		static SDL_Renderer* s_mainRenderer;
		static SDL_Window* s_window;

		// Miscellaneous (m)
		static string m_assetsFolder;
		static Game_AdvancedObject* m_keyboardInputObject;
		static TTF_Font* m_guiFont;

};

class Game_Tools
{
	public:
		// Object rendering
		static bool addGameObject(Game_Object* object, unsigned int layerID);
		static bool removeGameObject(Game_Object* object);

		// Misc
		static Game_Rect getTextSize(string text);

		template<typename ... Args>
		static string getAssetPath(string name, Args&... subDirectories);

		// Rendering
		static Game_RenderEquipment* createRenderEquipment(int surfaceWidth, int surfaceHeight);

		// TU functions
		static SDL_Surface* imageTextureObjectTU(Game_Object& object, Game_RenderEquipment* equipment);
};

template<typename T>
string combineStrings(T& firstString)
{
	return firstString;
}

template<typename T, typename ... Args>
string combineStrings(T& firstString, Args&... args)
{
	return firstString + "\\" + combineStrings(args...);
}

template<typename ... Args>
string Game_Tools::getAssetPath(string name, Args&... subDirectories)
{
	return Game_SharedMemory::m_assetsFolder + "\\" + combineStrings(subDirectories...) + "\\" + name;
}

#endif
