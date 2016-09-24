#ifndef GAME_CONTROL_H
#define GAME_CONTROL_H

#include <string>
#include "game_types.h"

// Control
int game_initialize(std::string windowTitle, Game_Rect windowSize, Game_Point windowStartPos);
void game_cleanup();

// Rendering
bool game_startRenderThread();
void game_stopRenderThread(bool join);

// Misc
void game_runMainLoop();

#endif
