#ifndef GAME_CONTROL_H
#define GAME_CONTROL_H

#include <SDL_image.h>

#include "shared.h"
#include "event.h"

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
