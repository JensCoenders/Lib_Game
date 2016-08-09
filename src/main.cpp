#include <iostream>
#include "testgame.h"

using namespace std;

int initialize()
{
	// Initialize SDL
	cout << "[INFO] Initializing SDL... ";

	int returnCode = game_initializeSDL("Jens Game V2.0");
	if (!returnCode)
		cout << "[OK]" << endl;
	else
	{
		switch (returnCode)
		{
			case -2:
				cout << "[CRIT]" << endl;
				cout << "[CRIT] Failed to initialize SDL! Error: " << SDL_GetError() << endl;
				break;
			case -3:
				cout << "[CRIT]" << endl;
				cout << "[CRIT] Failed to initialize SDL_image! Error: " << IMG_GetError() << endl;
				break;
			case -4:
				cout << "[CRIT]" << endl;
				cout << "[CRIT] Failed to initialize SDL_ttf! Error: " << TTF_GetError() << endl;
				break;
		}

		game_destroySDL();
		return returnCode;
	}

	return 0;
}

int main(int argc, char** argv)
{
	// TODO: Fix position glitch when window resizes

	// Initialize
	int result = initialize();
	if (result)
		return result;

	// Run game
	runTestGame();

	// Cleanup
	game_destroySDL();
	delete[] Game_SharedMemory::r_renderLayers;

	cout << "[INFO] Cleanup complete";
	return 0;
}
