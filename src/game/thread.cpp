#include <iostream>
#include <windows.h>

#include "thread.h"

using namespace std;

typedef struct game_threadpars
{
	public:
		int threadID;
		GAME_THREAD threadFunction;

} Game_ThreadPars;

/* Thread variables */

// IDs
bool threadAvailableList[GAME_MAX_THREADS];
HANDLE threadList[GAME_MAX_THREADS];

/* Thread functions */

void threadFunction(void* params);

int game_startThread(GAME_THREAD function)
{
	// Initialize list if not initialized
	static bool initialized = false;
	if (!initialized)
	{
		for (int i = 0; i < GAME_MAX_THREADS; i++)
		{
			threadAvailableList[i] = true;
		}
		initialized = true;
	}

	// Find ID
	int newThreadID = -1;
	for (int i = 0; i < GAME_MAX_THREADS; i++)
	{
		if (threadAvailableList[i])
		{
			threadAvailableList[i] = false;
			newThreadID = i;
			break;
		}
	}

	// Make sure thread limit isn't reached
	if (newThreadID == -1)
	{
		return -1;
	}

	// Create parameters
	Game_ThreadPars* pars = new Game_ThreadPars();
	pars->threadID = newThreadID;
	pars->threadFunction = function;

	// Start thread
	threadList[newThreadID] = (HANDLE)_beginthread(threadFunction, 0, pars);

	// Check if thread creation succeeded
	if (threadList[newThreadID] == NULL)
	{
		return -2;
	}

	return newThreadID;
}

void game_joinThread(int threadID)
{
	// Check if ID provides a valid thread
	if (threadID < 0 || threadID >= GAME_MAX_THREADS || threadAvailableList[threadID])
	{
		return;
	}

	// Wait for thread to finish
	WaitForSingleObject(threadList[threadID], INFINITE);
	CloseHandle(threadList[threadID]);
}

void threadFunction(void* pars)
{
	// Execute game thread function
	Game_ThreadPars* threadPars = (Game_ThreadPars*) pars;
	threadPars->threadFunction();

	// Reset thread ID
	threadAvailableList[threadPars->threadID] = true;
	delete threadPars;
}

void game_sleep(int milliseconds)
{
	Sleep(milliseconds);
}
