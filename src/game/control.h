#ifndef GAME_CONTROL_H
#define GAME_CONTROL_H 1

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "shared.h"

using namespace std;

/* Control functions */

// Rendering
void game_startRenderThread();
void game_joinRenderThread();

// Events
void game_processKeyboardEvent(SDL_Event* event);
void game_processMouseEvent(SDL_Event* event);
void game_processWindowEvent(SDL_Event* event);

// SDL
int game_initializeSDL(string windowTitle);
void game_destroySDL();

#endif
