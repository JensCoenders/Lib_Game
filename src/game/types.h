#ifndef GAME_TYPES_H
#define GAME_TYPES_H

#include <string>
#include <typeinfo>

using namespace std;

#define GAME_VERSION "0.2.0"

#define GAME_LAYER_GUI_FOREGROUND	0
#define GAME_LAYER_GUI_BACKGROUND	1
#define GAME_LAYER_LEVEL_FOREGROUND	2
#define GAME_LAYER_LEVEL_MID_1		3
#define GAME_LAYER_LEVEL_MID_2		4
#define GAME_LAYER_LEVEL_BACKGROUND	5
#define GAME_LAYER_BACKGROUND		6
#define GAME_LAYER_AMOUNT			7

#if SDL_BYTEORDER == SDL_BIG_ENDIAN

#define GAME_SURFACE_RMASK		0xff000000
#define GAME_SURFACE_GMASK 		0x00ff0000
#define GAME_SURFACE_BMASK 		0x0000ff00
#define GAME_SURFACE_AMASK 		0x000000ff

#else

#define GAME_SURFACE_RMASK		0x000000ff
#define GAME_SURFACE_GMASK		0x0000ff00
#define GAME_SURFACE_BMASK		0x00ff0000
#define GAME_SURFACE_AMASK		0xff000000

#endif

/* Type definitions */

typedef enum game_objecteventtype
{
	EVENT_TYPE_TYPED,
	EVENT_TYPE_CLICKED

} Game_ObjectEventType;

typedef struct game_point
{
		int x, y;
		string toString();

} Game_Point;

typedef struct game_rect
{
		int width, height;
		string toString();

} Game_Rect;

typedef struct game_camera
{
	public:
		Game_Point position;
		Game_Rect size;
		unsigned char movementDirection;
		int movementSpeed;

} Game_Camera;

typedef struct game_renderequipment
{
	public:
		SDL_Renderer* softwareRenderer;
		SDL_Surface* surface;

		game_renderequipment(SDL_Renderer* renderer, SDL_Surface* surface);
		~game_renderequipment();

} Game_RenderEquipment;

typedef struct game_objectproperty
{
	public:
		string name;

		int getIntValue();
		bool getBoolValue();
		string getStringValue();

		template <typename T>
		void setValue(T value);

		game_objectproperty();
		~game_objectproperty();
	private:
		int m_intValue;
		bool m_boolValue;
		string* m_stringValue;

} Game_ObjectProperty;

template <typename T>
void game_objectproperty::setValue(T value)
{
	if (typeid(T) == typeid(int))
		m_intValue = (int)value;
	else if (typeid(T) == typeid(bool))
		m_boolValue = (bool)value;
	else if (typeid(T) == typeid(string))
	{
		m_stringValue = new string();
		*m_stringValue = (string)value;
	}
	else
		cout << "[WARN] Data type other than int, bool or string not allowed for object property!" << endl;
}

template <typename T>
struct LinkedListNode
{
	public:
		T* value;
		LinkedListNode<T>* prevNode;
		LinkedListNode<T>* nextNode;

		LinkedListNode();
		~LinkedListNode();

};

template <typename T>
LinkedListNode<T>::LinkedListNode()
{
	value = NULL;
	prevNode = NULL;
	nextNode = NULL;
}

template <typename T>
LinkedListNode<T>::~LinkedListNode()
{
	if (prevNode)
		delete prevNode;

	if (nextNode)
		delete nextNode;
}

#endif
