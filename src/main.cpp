#include <iostream>
#include "testgame.h"

using namespace std;

int initialize()
{
#ifdef GAME_DEBUG
	gameVar_debugMode = true;
#endif

	GAME_DEBUG_CHECK
		cout << "[DEBUG] Initializing... ";

	// Initialize SDL
	string windowTitle = "Jens Game ";
	windowTitle += GAME_VERSION;

	int returnCode = game_initialize(windowTitle, {1024, 576}, {SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED});
	if (returnCode)
	{
		GAME_DEBUG_CHECK
			cout << "[CRIT]" << endl;

		switch (returnCode)
		{
			case -2:
				cout << "[ERROR] Failed to initialize SDL! Error: " << SDL_GetError() << endl;
				break;
			case -3:
				cout << "[ERROR] Failed to initialize SDL_image! Error: " << IMG_GetError() << endl;
				break;
			case -4:
				cout << "[ERROR] Failed to initialize SDL_ttf! Error: " << TTF_GetError() << endl;
				break;
		}

		game_cleanup();
		return returnCode;
	}
	else GAME_DEBUG_CHECK
		cout << "[OK]" << endl;

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
			gameVar_assetDir = argv[++i];

			GAME_DEBUG_CHECK
				cout << "[DEBUG] Setting assets folder to '" << gameVar_assetDir << "'" << endl;
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
	// Process arguments
	if (!processArguments(argc, argv))
		return -1;

	// Initialize
	int result = initialize();
	if (result)
		return result;

	// Run game
	runTestGame();

	// Clean up
	game_cleanup();

	GAME_DEBUG_CHECK
		cout << "[DEBUG] Cleanup complete" << endl;
	return 0;
}
