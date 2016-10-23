#ifndef GAME_THREAD_H
#define GAME_THREAD_H

#define GAME_MAX_THREADS 8

typedef void (*Game_ThreadRunFunc)();

typedef struct Game_ThreadPars
{
	public:
		int threadID;
		Game_ThreadRunFunc threadFunction;

} Game_ThreadPars;

int game_startThread(Game_ThreadRunFunc function);
void game_joinThread(int threadID);
void game_sleep(int milliseconds);

#endif
