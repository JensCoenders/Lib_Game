#include <iostream>
#include <windows.h>
#include "game_thread.h"

using namespace std;

HANDLE g_threads[GAME_MAX_THREADS];

void threadHandler(void* pars)
{
	// Execute game thread function
	Game_ThreadPars* threadPars = (Game_ThreadPars*) pars;
	threadPars->threadFunction();

	// Reset thread ID
	g_threads[threadPars->threadID] = NULL;
	delete threadPars;
}

int game_startThread(GAME_THREAD_FUNC function)
{
	// Initialize list if not initialized
	static bool initialized = false;
	if (!initialized)
	{
		for (int i = 0; i < GAME_MAX_THREADS; i++)
			g_threads[i] = NULL;

		initialized = true;
	}

	// Find ID
	int newThreadID = -1;
	for (int i = 0; i < GAME_MAX_THREADS; i++)
	{
		if (!g_threads[i])
		{
			newThreadID = i;
			g_threads[i] = NULL;
			break;
		}
	}

	// Make sure thread limit isn't reached
	if (newThreadID < 0)
		return -1;

	// Create parameters
	Game_ThreadPars* pars = new Game_ThreadPars();
	pars->threadID = newThreadID;
	pars->threadFunction = function;

	// Start thread
	g_threads[newThreadID] = (HANDLE)_beginthread(threadHandler, 0, pars);

	// Check if thread creation succeeded
	if (g_threads[newThreadID] == NULL)
		return -2;

	return newThreadID;
}

void game_joinThread(int threadID)
{
	// Check if ID provides a valid thread
	if (threadID < 0 || threadID >= GAME_MAX_THREADS || !g_threads[threadID])
		return;

	// Wait for thread to finish
	WaitForSingleObject(g_threads[threadID], INFINITE);
	CloseHandle(g_threads[threadID]);
}

void game_sleep(int milliseconds)
{
	Sleep(milliseconds);
}
