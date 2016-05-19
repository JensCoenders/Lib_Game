#include <iostream>

#include "game/controller.hpp"

using namespace std;

int main(int argc, char** argv)
{
	Game_Controller* controller = new Game_Controller();

	// Initialize SDL
	cout << "[INFO] Initializing SDL... ";
	Game_Result result = controller->initializeSDL("Jens Game V2.0");
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

	// Setup game
	controller->setProperty(PROPERTY_USE_FPS_COUNTER, true);
	controller->gameLoop();

	// Cleanup
	cout << "[INFO] Cleaning up... ";
	try
	{
		controller->destroySDL();
		delete controller;
		cout << "[OK]" << endl;
	}
	catch (exception& e)
	{
		cout << "[ERR]" << endl;
		cout << "[ERR] Error while cleaning up: " << e.what() << endl;
	}

	return GAME_SUCCESS;
}
