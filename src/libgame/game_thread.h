#ifndef GAME_THREAD_H
#define GAME_THREAD_H

#define GAME_MAX_THREADS 8

typedef void (*GAME_THREAD_FUNC) ();

typedef struct Game_ThreadPars
{
	public:
		int threadID;
		GAME_THREAD_FUNC threadFunction;

} Game_ThreadPars;

int game_startThread(GAME_THREAD_FUNC function);
void game_joinThread(int threadID);
void game_sleep(int milliseconds);

#endif
