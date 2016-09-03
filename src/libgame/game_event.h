#ifndef GAME_EVENT_H
#define GAME_EVENT_H

#include <SDL2/SDL.h>

void game_processKeyboardEvent(SDL_Event& event);
void game_processMouseEvent(SDL_Event& event);
void game_processWindowEvent(SDL_Event& event);

#endif
