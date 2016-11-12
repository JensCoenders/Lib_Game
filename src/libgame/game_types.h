#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include <string>
#include <SDL2/SDL.h>

/* General */

typedef struct Game_Point
{
	public:
		int x, y;
		std::string toString();

} Game_Point;

typedef struct Game_Rect
{
	public:
		int width, height;
		std::string toString();

} Game_Rect;

/* Events */

typedef enum Game_ObjectEventType
{
	EVENT_TYPE_TYPED,
	EVENT_TYPE_CLICKED

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
		int relX, relY;

		Game_MouseClickedEvent(SDL_Event* event);

} Game_MouseClickedEvent;

/* Rendering */

typedef struct Game_RenderEquipment
{
	public:
		SDL_Renderer* softwareRenderer;
		SDL_Surface* surface;

		Game_RenderEquipment(SDL_Renderer* renderer, SDL_Surface* surface);
		~Game_RenderEquipment();

} Game_RenderEquipment;

/* Misc */

typedef struct Game_Asset
{
	public:
		std::string assetPath;
		SDL_Surface* loadedSurface;

		Game_Asset(std::string assetPath, SDL_Surface* loadedSurface);
		~Game_Asset();

} Game_Asset;

typedef struct Game_ObjectProperty
{
	public:
		std::string name;

		int getIntValue();
		bool getBoolValue();
		std::string getStringValue();

		void setValue(int value);
		void setValue(bool value);
		void setValue(std::string value);

		Game_ObjectProperty();
		~Game_ObjectProperty();

	private:
		int m_intValue;
		bool m_boolValue;
		std::string* m_stringValue;

} Game_ObjectProperty;

#endif
