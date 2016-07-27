#ifndef GAME_OBJ_H
#define GAME_OBJ_H 1

#include <SDL.h>
#include <string>
#include <typeinfo>

using namespace std;

/* Type definitions */

typedef struct game_point
{
		unsigned int x, y;
		string toString();

} Game_Point;

typedef struct game_rect
{
		unsigned int width, height;
		string toString();

} Game_Rect;

typedef struct game_objecttudata
{
	public:
		Game_Rect bounds;
		SDL_Surface* surface;
		SDL_Renderer* softwareRenderer;

} Game_ObjectTUData;

template<typename T>
struct Game_ObjectProperty;

template<typename T>
struct Game_ObjectProperty
{
	public:
		string name;
		T value;
		Game_ObjectProperty* nextProperty;

};

typedef enum game_objecteventtype
{
	TYPE_KEY_TYPED,
	TYPE_MOUSE_CLICKED

} Game_ObjectEventType;

class Game_Object;
typedef void (*Game_ObjectFUFunc)(Game_Object* object);
typedef void (*Game_ObjectTUFunc)(Game_Object* object, Game_Rect bounds, SDL_Renderer* renderer);
typedef void (*Game_ObjectEventFunc)(Game_Object* object, SDL_Event* eventData);

// TODO: Create setColor function
class Game_Object
{
	public:
		// World variables
		Game_Point m_worldCoords;
		Game_Rect m_worldSize;

		// Texture variables
		bool m_needsTextureUpdate;
		SDL_Texture* m_lastRenderedTexture;

		// Update functions
		void frameUpdate();
		void setFrameUpdate(Game_ObjectFUFunc function);
		void textureUpdate(Game_Rect bounds, SDL_Renderer* renderer);
		void setTextureUpdate(Game_ObjectTUFunc function);

		// Texture functions
		void requestTextureUpdate();
		bool needsTextureUpdate();

		// Event functions
		bool callEventFunction(Game_ObjectEventType type, SDL_Event* event);
		void setEventFunction(Game_ObjectEventType type, Game_ObjectEventFunc function);

		// Property functions
		template<typename T>
		T getProperty(string name, T defaultValue);

		template<typename T>
		void setProperty(string name, T value);

		// Miscellaneous functions
		unsigned int getID();

		Game_Object(int worldX, int worldY, int worldWidth, int worldHeight);
		~Game_Object();
	private:
		// Update function variables
		Game_ObjectFUFunc m_frameUpdateFunc;
		Game_ObjectTUFunc m_textureUpdateFunc;

		// Event function variables
		Game_ObjectEventFunc m_keyTypedFunc;
		Game_ObjectEventFunc m_mouseClickedFunc;

		// Property variables
		Game_ObjectProperty<string>* m_stringProperties;
		Game_ObjectProperty<int>* m_integerProperties;
		// TODO: Create m_booleanProperties

		// Miscellaneous variables
		unsigned int m_ID;

};

template<typename T>
T Game_Object::getProperty(string name, T defaultValue)
{
	for (unsigned int i = 0; i < name.length(); i++)
	{
		name[i] = tolower(name.at(i));
	}

	Game_ObjectProperty<T>* currentProperty = NULL;
	if (typeid(T) == typeid(int))
		currentProperty = (Game_ObjectProperty<T>*) m_integerProperties;
	else if (typeid(T) == typeid(string))
		currentProperty = (Game_ObjectProperty<T>*) m_stringProperties;

	while (currentProperty)
	{
		if (name == currentProperty->name)
		{
			return currentProperty->value;
		}
		currentProperty = currentProperty->nextProperty;
	}

	return defaultValue;
}

template<typename T>
void Game_Object::setProperty(string name, T value)
{
	for (unsigned int i = 0; i < name.length(); i++)
	{
		name[i] = tolower(name.at(i));
	}

	Game_ObjectProperty<T>** baseProperty = NULL;
	if (typeid(T) == typeid(int))
		baseProperty = (Game_ObjectProperty<T>**) &m_integerProperties;
	else if (typeid(T) == typeid(string))
		baseProperty = (Game_ObjectProperty<T>**) &m_stringProperties;

	Game_ObjectProperty<T>* currentProperty = *baseProperty;
	while (currentProperty)
	{
		if (name == currentProperty->name)
		{
			currentProperty->value = value;
			break;
		}
		currentProperty = currentProperty->nextProperty;
	}

	if (!currentProperty)
	{
		Game_ObjectProperty<T>* newProperty = new Game_ObjectProperty<T>();
		newProperty->name = name;
		newProperty->value = value;
		newProperty->nextProperty = *baseProperty;
		*baseProperty = newProperty;
	}
}

#endif
