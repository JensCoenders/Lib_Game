#include "game.h"

void game(Game_Controller* controller)
{
	// Setup game
	game_setProperty(PROPERTY_USE_FPS_COUNTER, true);
	controller->startGameLoop();
}
