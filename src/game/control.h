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

/* Type definitions */

typedef struct game_result
{
	public:
		int returnCode;
		string message;

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

/* Shared Memory */

class Game_SharedMemory
{
	public:
		// Properties (p)
		static bool p_running;
		static int p_targetFPS;
		static bool p_useFPSCounter;
		static float p_zoomScale;

		// Rendering (r)
		static Game_Point r_cameraPos;
		static Game_Rect r_cameraBounds;
		static Game_Layer* r_layers;
		static int r_renderThreadID;

		// SDL (s)
		static bool s_SDLInitialized;
		static SDL_Renderer* s_mainRenderer;
		static SDL_Window* s_window;

		// Miscellaneous (m)
		static Game_Object* m_keyboardInputObject;

		// Functions
		static bool startRenderingObject(Game_Object* object, unsigned int layerID);
		static bool stopRenderingObject(Game_Object* object);

};

/* Control functions */

// SDL
Game_Result game_initializeSDL(string windowTitle);
void game_destroySDL();

// Rendering
void game_startRenderThread();
void game_joinRenderThread();

// Events
void game_processKeyboardEvent(SDL_Event* event);
void game_processMouseEvent(SDL_Event* event);
void game_processWindowEvent(SDL_Event* event);

#endif
