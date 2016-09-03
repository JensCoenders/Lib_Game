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
// TODO: Create structure for keyboard event

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

typedef enum Game_RenderMode
{
	RENDERMODE_STATIC_BACKGROUND,
	RENDERMODE_IMAGE_TEXTURE,
	RENDERMODE_CUSTOM

} Game_RenderMode;

struct Game_RenderPars
{
	public:
		Game_RenderMode renderMode;

		Game_RenderPars(Game_RenderMode renderMode = RENDERMODE_CUSTOM);
		virtual ~Game_RenderPars();

};

typedef struct Game_RP_StaticBackground : public Game_RenderPars
{
	public:
		SDL_Color backgroundColor;

		Game_RP_StaticBackground(SDL_Color backgroundColor);

} Game_RP_StaticBackground;

typedef struct Game_RP_ImageTexture : public Game_RenderPars
{
	public:
		std::string imageTexturePath;

		Game_RP_ImageTexture(std::string imageTexturePath);

} Game_RP_ImageTexture;

typedef struct Game_RenderEquipment
{
	public:
		SDL_Renderer* softwareRenderer;
		SDL_Surface* surface;

		Game_RenderEquipment(SDL_Renderer* renderer, SDL_Surface* surface);
		~Game_RenderEquipment();

} Game_RenderEquipment;

/* Misc */

typedef struct Game_Camera
{
	public:
		Game_Point position;
		Game_Rect size;
		unsigned char movementDirection;
		int movementSpeed;

} Game_Camera;

typedef enum Game_ObjectEventType
{
	EVENT_TYPE_TYPED,
	EVENT_TYPE_CLICKED

} Game_ObjectEventType;

typedef struct Game_ObjectProperty
{
	public:
		std::string name;

		int getIntValue();
		bool getBoolValue();
		std::string getStringValue();

		template<typename T>
		void setValue(T value);

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

template<typename T>
void Game_ObjectProperty::setValue(T value)
{
	if (typeid(T) == typeid(int))
		m_intValue = (int) value;
	else if (typeid(T) == typeid(bool))
		m_boolValue = (bool) value;
	else if (typeid(T) == typeid(std::string))
		m_stringValue = new std::string(value);
	else
		std::cout << "[WARN] Data type other than int, bool or string not allowed for object property!" << std::endl;
}

#endif
