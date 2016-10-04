#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include <iostream>
#include <string>
#include <typeinfo>
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

template<typename T>
struct LinkedListNode
{
	public:
		T* value;
		LinkedListNode<T>* prevNode;
		LinkedListNode<T>* nextNode;

		LinkedListNode();
		~LinkedListNode();

};

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

typedef enum Game_ObjectFloatMode
{
	FLOAT_LEFT_TOP,
	FLOAT_LEFT_CENTER,
	FLOAT_LEFT_BOTTOM,
	FLOAT_CENTER_TOP,
	FLOAT_CENTER,
	FLOAT_CENTER_BOTTOM,
	FLOAT_RIGHT_TOP,
	FLOAT_RIGHT_CENTER,
	FLOAT_RIGHT_BOTTOM

} Game_ObjectFloatMode;

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

/* Function definitions */

template<typename T>
LinkedListNode<T>::LinkedListNode()
{
	value = NULL;
	prevNode = NULL;
	nextNode = NULL;
}

template<typename T>
LinkedListNode<T>::~LinkedListNode()
{
	if (prevNode)
		delete prevNode;

	if (nextNode)
		delete nextNode;
}

#endif
