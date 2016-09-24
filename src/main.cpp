#include <iostream>
#include "testgame.h"

using namespace std;

int initialize()
{
	// Initialize SDL
	string windowTitle = "Jens Game ";
	windowTitle += GAME_VERSION;

	int returnCode = game_initialize(windowTitle, {1024, 576}, {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED});
	if (returnCode)
	{
		switch (returnCode)
		{
			case -2:
				cout << "[CRIT] Failed to initialize SDL! Error: " << SDL_GetError() << endl;
				break;
			case -3:
				cout << "[CRIT] Failed to initialize SDL_image! Error: " << IMG_GetError() << endl;
				break;
			case -4:
				cout << "[CRIT] Failed to initialize SDL_ttf! Error: " << TTF_GetError() << endl;
				break;
		}

		game_cleanup();
		return returnCode;
	}

	return 0;
}

void displayHelp(string programName)
{
	cout << "Usage: " << programName << " [options]" << endl;
	cout << "--assets <path>\t\tSpecify a different assets directory." << endl;
	cout << "This software was developed by Jens Coenders" << endl;
}

bool processArguments(int argc, char** argv)
{
	for (int i = 1; i < argc; i++)
	{
		string argument = argv[i];
		for (unsigned int j = 0; j < argument.size(); j++)
			argument[j] = tolower(argument[j]);

		if (argument == "--assets" && (i + 1) < argc)
		{
			game_shmPut(SHM_ASSETS_DIR, argv[++i]);
			cout << "[INFO] Setting assets folder to '" << game_shmGet(SHM_ASSETS_DIR) << "'" << endl;
		}
		else if (argument == "--help")
		{
			displayHelp(argv[0]);
			return false;
		}
		else
		{
			cout << "Unknown argument '" << argument << "'!" << endl;
			displayHelp(argv[0]);
			return false;
		}
	}

	return true;
}

int main(int argc, char** argv)
{
	// TODO: Fix position glitch when window resizes
	// TODO: Fix random window crash when window resizes
	// TODO: Create parent-child system

	// Process arguments
	if (!processArguments(argc, argv))
		return -1;

	// Initialize
	int result = initialize();
	if (result)
		return result;

	// Run game
	runTestGame();
	cout << "[INFO] Stopped main thread" << endl;

	// Cleanup
	game_cleanup();

	cout << "[INFO] Cleanup complete" << endl;
	return 0;
}
