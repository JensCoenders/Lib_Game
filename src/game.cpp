#include "game.h"

void game_run()
{
	// Setup game
	game_sharedMemory.p_useFPSCounter = true;

	// Run game
	game_startMainThread();
}
