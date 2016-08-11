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
typedef SDL_Surface* (*Game_ObjectTUFunc)(Game_Object* object, Game_RenderEquipment* equipment);
typedef void (*Game_ObjectEventFunc)(Game_Object* object, SDL_Event& eventData);

class Game_Object
{
	public:
		// Texture vars
		SDL_Texture* lastRenderedTexture;

		// World funcs
		Game_Point getCoords();
		Game_Rect getSize();
		void setCoords(int x, int y);
		void setSize(int width, int height);

		// Update funcs
		void frameUpdate();
		SDL_Surface* textureUpdate(Game_RenderEquipment* equipment);
		void setFrameUpdate(Game_ObjectFUFunc function);
		void setTextureUpdate(Game_ObjectTUFunc function);

		// Texture update funcs
		bool needsTextureUpdate();
		void requestTextureUpdate();
		void satisfyTextureUpdate();

		// Event funcs
		bool callEventFunction(Game_ObjectEventType type, SDL_Event& event);
		void setEventFunction(Game_ObjectEventType type, Game_ObjectEventFunc function);

		// Property funcs
		int getIntProperty(string name, int defaultValue);
		bool getBoolProperty(string name, bool defaultValue);
		string getStringProperty(string name, string defaultValue);

		template<typename T>
		void setProperty(string name, T value);

		// Misc functions
		unsigned int getID();
		Game_ObjectType getType();
		void setType(Game_ObjectType type);

		Game_Object(int x, int y, int w, int h, Game_ObjectType type = OBJECT_TYPE_NORMAL);
		~Game_Object();
	protected:
		// World vars
		Game_Point m_coords;
		Game_Rect m_size;

		// Texture vars
		bool m_needsTextureUpdate;

		// Update & event vars
		Game_ObjectFUFunc m_frameUpdateFunc;
		Game_ObjectTUFunc m_textureUpdateFunc;
		Game_ObjectEventFunc m_keyTypedFunc;
		Game_ObjectEventFunc m_mouseClickedFunc;

		// Property vars
		Game_ObjectProperty* m_properties;

		// Misc vars
		unsigned int m_ID;
		Game_ObjectType m_objectType;

		// Misc funcs
		Game_ObjectProperty* findPropertyByName(string name);

};

template<typename T>
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

class Game_TextObject : public Game_Object
{
	public:
		// Text vars
		double textScaling;
		bool autoSize;

		// Text funcs
		string getText();
		void setText(string text);
		SDL_Surface* renderText();

		// Color funcs
		SDL_Color getTextColor();
		SDL_Color getBackgroundColor();
		void setTextColor(SDL_Color color);
		void setBackgroundColor(SDL_Color color);

		Game_TextObject(int x, int y, int w, int h, Game_ObjectType type = OBJECT_TYPE_NORMAL);
	private:
		string m_text;
		SDL_Color m_textColor;
		SDL_Color m_backgroundColor;
};

#endif
