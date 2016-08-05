#include <iostream>
#include "game.h"

using namespace std;

int main(int argc, char** argv)
{
	// TODO: Fix rendering freeze when window resizes

	// Initialize SDL
	cout << "[INFO] Initializing SDL... ";
	int returnCode = game_initializeSDL("Jens Game V2.0");
	if (!returnCode)
		cout << "[OK]" << endl;
	else if (returnCode == -1)
		cout << "[WARN]" << endl << "[WARN] SDL has already been initialized!" << endl;
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
			default:
				cout << "[WARN]" << endl;
				cout << "[WARN] Unrecognized return code: " << returnCode << endl;
				break;
		}

		game_destroySDL();
		return returnCode;
	}

	// Call game startup
	game_run();

	// Cleanup
	game_destroySDL();
	cout << "[INFO] Cleanup complete";

	return 0;
}
