#include "game.h"

void game_run()
{
	// Setup game
	Game_SharedMemory::p_useFPSCounter = true;

	// Run game
	game_startMainThread();
}
