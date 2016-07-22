#include <iostream>
#include "game.h"

using namespace std;

int main(int argc, char** argv)
{
	// Initialize SDL
	cout << "[INFO] Initializing SDL... ";
	Game_Result result = game_initializeSDL("Jens Game V2.0");
	switch (result.returnCode)
	{
		case GAME_SUCCESS:
			cout << "[OK]" << endl;
			break;
		case GAME_ERR_ALREADY_INIT:
			cout << "[WARN]" << endl;
			cout << "[INFO] SDL has already been initialized!?" << endl;
			break;
		case GAME_CRIT_SDL:
			cout << "[CRIT]" << endl;
			cout << "[CRIT] Failed to initialize SDL!" << endl;
			cout << "[CRIT] Error: " << result.message << endl;
			return GAME_CRIT_SDL;
		case GAME_CRIT_SDL_IMG:
			cout << "[CRIT]" << endl;
			cout << "[CRIT] Failed to initialize SDL image!" << endl;
			cout << "[CRIT] Error: " << result.message << endl;
			return GAME_CRIT_SDL_IMG;
		default:
			cout << "[WARN]" << endl;
			cout << "[WARN] Unrecognized return code: " << result.returnCode << endl;
			break;
	}

	// Call game startup
	game_run();

	// Cleanup
	cout << "[INFO] Cleaning up... ";
	try
	{
		game_destroySDL();
		cout << "[OK]" << endl;
	}
	catch (exception& e)
	{
		cout << "[ERR]" << endl;
		cout << "[ERR] Error while cleaning up: " << e.what() << endl;
	}

	return GAME_SUCCESS;
}
