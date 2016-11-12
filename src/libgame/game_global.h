#ifndef GAME_SHM_H
#define GAME_SHM_H

#include <string>
#include <SDL2/SDL_ttf.h>
#include "game_defs.h"
#include "game_object.h"
#include "game_types.h"
#include "game_utils.hpp"

extern bool gameVar_isRunning;
extern int gameVar_targetFps;
extern bool gameVar_useFpsCounter;
extern bool gameVar_debugMode;

extern std::string gameVar_assetDir;
extern LinkedList<Game_Asset, std::string> gameVar_loadedAssets;

extern Game_Camera gameVar_mainCamera;
extern bool gameVar_keyboardMovesCamera;
extern double gameVar_zoomScale;

extern Game_RenderLayer* gameVar_renderLayers;

extern bool gameVar_SDLInitialized;
extern SDL_Renderer* gameVar_mainRenderer;
extern SDL_Window* gameVar_mainWindow;

extern TTF_Font* gameVar_guiFont;
extern Game_Object* gameVar_fpsObject;
extern Game_Object* gameVar_keyboardInputObject;

#endif
