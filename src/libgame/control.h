#ifndef GAME_CONTROL_H
#define GAME_CONTROL_H

#include <SDL2/SDL_image.h>

#include "event.h"
#include "shared.h"

using namespace std;

/* Control functions */

// Main
void game_mainLoop();

// Rendering
void game_startRenderThread();
void game_joinRenderThread();

// SDL
int game_initializeSDL(string windowTitle);
void game_destroySDL();

#endif
