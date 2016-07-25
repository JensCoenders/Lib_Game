#ifndef GAME_OBJ_H
#define GAME_OBJ_H 1

#include <string>
#include <SDL.h>

#define GAME_OBJ_MAX_QUEUED_EVENTS 100

typedef struct game_point
{
		int x, y;
		std::string toString();

} Game_Point;

typedef struct game_rect
{
		int width, height;
		std::string toString();

} Game_Rect;

class Game_Object;
typedef void (*Game_ObjectFrameUpdateFunc)(Game_Object* object);
typedef SDL_Texture* (*Game_ObjectCompileTextureFunc)(Game_Object* object, Game_Rect realBounds, SDL_Surface* surface, SDL_Renderer* renderer);

class Game_Object
{
	public:
		unsigned int getID();

		Game_Point m_worldCoords;
		Game_Rect m_worldSize;

		bool m_needsTextureUpdate;
		SDL_Texture* m_lastRenderedTexture;
		Game_ObjectCompileTextureFunc m_compileTextureFunc;

		Game_ObjectFrameUpdateFunc m_frameUpdateFunc;

		Game_Object(int worldX, int worldY, int worldWidth, int worldHeight);
		virtual ~Game_Object()
		{
		}
	private:
		unsigned int m_ID;

};

class Game_EventObject : public Game_Object
{
	public:
		unsigned short int m_newKeyboardEvents;
		SDL_KeyboardEvent* pollKeyboardEvent();

		unsigned short int m_newMouseEvents;
		SDL_MouseButtonEvent* pollMouseEvent();

		void addKeyboardEvent(SDL_KeyboardEvent* event);
		void addMouseEvent(SDL_MouseButtonEvent* event);

		Game_EventObject(int worldX, int worldY, int worldWidth, int worldHeight);
		~Game_EventObject();
	private:
		SDL_KeyboardEvent** m_queuedKeyboardEvents;
		SDL_MouseButtonEvent** m_queuedMouseEvents;

		SDL_Event* pollEvent(SDL_Event** queue);
};

#endif
