#ifndef GAME_TYPES_H
#define GAME_TYPES_H 1

#include <string>

using namespace std;

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

typedef enum game_objecttype
{
	OBJECT_TYPE_NORMAL, OBJECT_TYPE_GUI, OBJECT_TYPE_WORLD

} Game_ObjectType;

typedef enum game_objecteventtype
{
	EVENT_TYPE_KEY, EVENT_TYPE_MOUSE

} Game_ObjectEventType;

typedef struct game_point
{
		unsigned int x, y;
		string toString();

} Game_Point;

typedef struct game_rect
{
		int width, height;
		string toString();

} Game_Rect;

typedef struct game_objectproperty Game_ObjectProperty;
typedef struct game_objectproperty
{
	public:
		string m_name;
		Game_ObjectProperty* m_nextProperty;

		int* m_intValue;
		bool* m_boolValue;
		string* m_stringValue;

		template <typename T>
		void setValue(T value);

		game_objectproperty();
		~game_objectproperty();

} Game_ObjectProperty;

#endif
