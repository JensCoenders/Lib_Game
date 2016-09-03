#ifndef GAME_THREAD_H
#define GAME_THREAD_H

#define GAME_MAX_THREADS 8

typedef void (*GAME_THREAD) ();

int game_startThread(GAME_THREAD function);
void game_joinThread(int threadID);
void game_sleep(int milliseconds);

#endif
