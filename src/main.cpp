#include <iostream>
#include "game/controller.h"

using namespace std;

int main(int argc, char** argv)
{
	Game_Controller* controller = new Game_Controller();

	// Initialize SDL
	cout << "Initializing SDL... ";
	Game_ErrorMsg result = controller->initializeSDL("Jens Game V2.0");
	switch (result.errorCode)
	{
		case GAME_SUCCESS:
			cout << "[OK]" << endl;
			break;
		case GAME_ERR_ALREADY_INIT:
			cout << "[WARN]" << endl;
			cout << "SDL has already been initialized!?" << endl;
			break;
		case GAME_CRIT_SDL:
			cout << "[FAIL]" << endl;
			cout << "Failed to initialize SDL!" << endl;
			cout << "Error: " << result.message << endl;
			return GAME_CRIT_SDL;
		case GAME_CRIT_SDL_IMG:
			cout << "[FAIL]" << endl;
			cout << "Failed to initialize SDL image!" << endl;
			cout << "Error: " << result.message << endl;
			return GAME_CRIT_SDL_IMG;
		default:
			cout << "[WARN]" << endl;
			cout << "Unrecognized return code: " << result.errorCode << endl;
			break;
	}

	// Run game loop
	controller->gameLoop();

	// Cleanup
	controller->destroySDL();
	delete controller;

	return GAME_SUCCESS;
}
