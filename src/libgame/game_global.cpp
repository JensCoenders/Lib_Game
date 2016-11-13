#include "game_defs.h"
#include "game_global.h"

using namespace std;

bool gameVar_isRunning = true;
int gameVar_targetFps = 60;
bool gameVar_useFpsCounter = true;
bool gameVar_debugMode = false;

std::string gameVar_assetDir = "assets";
LinkedList<Game_Asset, string> gameVar_loadedAssets;

Game_Camera gameVar_mainCamera = {{0, 0}, {0, 0}, NULL, 0, 2};
bool gameVar_keyboardMovesCamera = true;
double gameVar_zoomScale = 1.0;

Game_RenderLayer* gameVar_renderLayers = new Game_RenderLayer[GAME_LAYER_AMOUNT];

bool gameVar_SDLInitialized = false;
SDL_Renderer* gameVar_mainRenderer = NULL;
SDL_Window* gameVar_mainWindow = NULL;

TTF_Font* gameVar_guiFont = NULL;
Game_Object* gameVar_fpsObject = NULL;
Game_Object* gameVar_keyboardInputObject = NULL;
