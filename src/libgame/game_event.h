#ifndef GAME_EVENT_H
#define GAME_EVENT_H

#include <SDL2/SDL.h>

void game_processKeyboardEvent(SDL_Event& event);
void game_processMouseEvent(SDL_Event& event);
void game_processWindowEvent(SDL_Event& event);

typedef enum Game_ObjectEventType
{
	EVENT_TYPE_TYPED,
	EVENT_TYPE_CLICKED,
	EVENT_TYPE_MOTION

} Game_ObjectEventType;

typedef struct Game_ObjectEvent
{
	public:
		SDL_Event* originalEvent;

		Game_ObjectEvent(SDL_Event* event);

} Game_ObjectEvent;

typedef struct Game_KeyTypedEvent : public Game_ObjectEvent
{
	public:
		SDL_Scancode scancode;
		bool pressed;
		unsigned short keyMod;
		short repeat;

		Game_KeyTypedEvent(SDL_Event* event);

} Game_KeyTypedEvent;

typedef struct Game_MouseClickedEvent : public Game_ObjectEvent
{
	public:
		int button;
		bool pressed;
		int relativeX, relativeY;

		Game_MouseClickedEvent(SDL_Event* event);

} Game_MouseClickedEvent;

typedef struct Game_MouseHoveredEvent : public Game_ObjectEvent
{
	public:
		bool entering;
		int newX, newY;
		int movedX, movedY;

		Game_MouseHoveredEvent(SDL_Event* event);

} Game_MouseHoveredEvent;

#endif
