#ifndef GAME_CONTROL_H
#define GAME_CONTROL_H 1

#include <iostream>
#include <string>
#include <SDL.h>
#include "object.h"

#define GAME_SUCCESS 				0
#define GAME_ERR_ALREADY_INIT		1
#define GAME_CRIT_SDL 				-1
#define GAME_CRIT_SDL_IMG			-2

#define GAME_LAYER_AMOUNT			7

using namespace std;

/* Shared Memory */

class Game_SharedMemory
{
	public:
		// Properties (p)
		static bool p_running;
		static bool p_useFPSCounter;
		static float p_zoomScale;

		// Rendering (r)
		static bool r_SDLInitialized;
		static Game_Layer* r_layers;
		static SDL_Window* r_window;
		static SDL_Renderer* r_windowRenderer;

};

/* Type definitions */

typedef struct game_result
{
	public:
		int returnCode;
		std::string message;

} Game_Result;

typedef struct game_objectnode Game_ObjectNode;
typedef struct game_objectnode
{
	public:
		Game_ObjectNode* prevNode;
		Game_ObjectNode* nextNode;
		Game_Object* object;

		game_objectnode();
		~game_objectnode();

} Game_ObjectNode;

typedef struct game_layer
{
	public:
		int objectCount;
		Game_ObjectNode* objectList;

		game_layer();
		~game_layer();

} Game_Layer;

/* Control functions */

// SDL
Game_Result game_initializeSDL(std::string windowTitle);
void game_destroySDL();

// Rendering
void game_startMainThread();
void game_stopMainThread();

#endif
