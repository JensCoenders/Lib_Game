#ifndef GAME_OBJ_H
#define GAME_OBJ_H 1

#include <SDL.h>
#include <string>
#include <typeinfo>
#include "types.h"

using namespace std;

/* Type definitions */

class Game_Object;
typedef struct game_objectnode Game_ObjectNode;
typedef struct game_objectnode
{
	public:
		Game_ObjectNode* prevNode;
		Game_ObjectNode* nextNode;
		Game_Object* object;

		game_objectnode();
		~game_objectnode();

} Game_ObjectNode;

typedef struct game_renderlayer
{
	public:
		int objectCount;
		Game_ObjectNode* objectList;

		game_renderlayer();
		~game_renderlayer();

} Game_RenderLayer;

typedef void (*Game_ObjectFUFunc)(Game_Object* object);
typedef void (*Game_ObjectTUFunc)(Game_Object* object, SDL_Surface* surface, SDL_Renderer* renderer);
typedef void (*Game_ObjectEventFunc)(Game_Object* object, SDL_Event& eventData);

class Game_Object
{
	public:
		// Texture variables
		bool m_needsTextureUpdate;
		Game_Rect m_textureSize;
		SDL_Texture* m_lastRenderedTexture;

		// World functions
		Game_Point getCoords();
		void setCoords(int x, int y);
		Game_Rect getSize();
		void setSize(int width, int height);

		// Update functions
		void frameUpdate();
		void setFrameUpdate(Game_ObjectFUFunc function);
		void textureUpdate(SDL_Surface* surface, SDL_Renderer* renderer);
		void setTextureUpdate(Game_ObjectTUFunc function);

		// Texture functions
		void requestTextureUpdate();
		bool needsTextureUpdate();

		// Event functions
		bool callEventFunction(Game_ObjectEventType type, SDL_Event& event);
		void setEventFunction(Game_ObjectEventType type, Game_ObjectEventFunc function);

		// Property functions
		int getIntProperty(string name, int defaultValue);
		bool getBoolProperty(string name, bool defaultValue);
		string getStringProperty(string name, string defaultValue);

		template <typename T>
		void setProperty(string name, T value);

		// Miscellaneous functions
		unsigned int getID();
		Game_ObjectType getType();

		Game_Object(int x, int y, int w, int h);
		~Game_Object();
	protected:
		// World variables
		Game_Point m_coords;
		Game_Rect m_size;

		// Update function variables
		Game_ObjectFUFunc m_frameUpdateFunc;
		Game_ObjectTUFunc m_textureUpdateFunc;

		// Event function variables
		Game_ObjectEventFunc m_keyTypedFunc;
		Game_ObjectEventFunc m_mouseClickedFunc;

		// Property variables
		Game_ObjectProperty* m_properties;

		// Miscellaneous variables
		unsigned int m_ID;
		Game_ObjectType m_objectType;

		// Property functions
		Game_ObjectProperty* findPropertyByName(string name);

};

template <typename T>
void Game_Object::setProperty(string name, T value)
{
	for (unsigned int i = 0; i < name.length(); i++)
		name[i] = tolower(name.at(i));

	Game_ObjectProperty* currentProperty = findPropertyByName(name);
	if (!currentProperty)
	{
		currentProperty = new Game_ObjectProperty();
		currentProperty->m_name = name;
		currentProperty->m_nextProperty = m_properties;
		m_properties = currentProperty;
	}

	if (typeid(T) == typeid(int))
	{
		if (!currentProperty->m_intValue)
			currentProperty->m_intValue = new int();

		*currentProperty->m_intValue = value;
	}
	else if (typeid(T) == typeid(bool))
	{
		if (!currentProperty->m_boolValue)
			currentProperty->m_boolValue = new bool();

		*currentProperty->m_boolValue = value;
	}
	else if (typeid(T) == typeid(string))
	{
		if (!currentProperty->m_stringValue)
			currentProperty->m_stringValue = new string();

		*currentProperty->m_stringValue = value;
	}
}

class Game_GUIObject : public Game_Object
{
	public:
		// Text
		string getText();
		void setText(string text);

		SDL_Color getTextColor();
		void setTextColor(SDL_Color color);

		SDL_Color getBackgroundColor();
		void setBackgroundColor(SDL_Color color);

		Game_GUIObject(int x, int y, int w, int h);
	private:
		string m_text;
		SDL_Color m_textColor;
		SDL_Color m_backgroundColor;
};

class Game_WorldObject : public Game_Object
{
	public:
		Game_WorldObject(int worldX, int worldY, int worldWidth, int worldHeight);
};

#endif
