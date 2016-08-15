#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <SDL.h>

#include "types.h"

using namespace std;

/* Type definitions */

class Game_Object;

typedef struct game_renderlayer
{
	public:
		int objectCount;
		LinkedListNode<Game_Object>* objectList;

		game_renderlayer();
		~game_renderlayer();

} Game_RenderLayer;

typedef void (*Game_ObjectFUFunc)(Game_Object& object);
typedef SDL_Surface* (*Game_ObjectTUFunc)(Game_Object& object, Game_RenderEquipment* equipment);

class Game_Object
{
	public:
		// Misc vars
		bool isStatic;

		// Texture vars
		SDL_Texture* lastRenderedTexture;

		// World vars
		Game_Point worldCoords;
		Game_Rect worldSize;

		// Misc funcs
		unsigned int getID();

		// Texture funcs
		SDL_Color getBackgroundColor();
		string getImageTexturePath();
		void setBackgroundColor(SDL_Color color);
		void setImageTexture(string textureName);

		// Texture update funcs
		bool needsTextureUpdate();
		void requestTextureUpdate();
		void satisfyTextureUpdate();

		// Update funcs
		void frameUpdate();
		SDL_Surface* textureUpdate(Game_RenderEquipment* equipment);
		void setFrameUpdate(Game_ObjectFUFunc function);
		void setTextureUpdate(Game_ObjectTUFunc function);

		Game_Object(int x, int y, int w, int h, bool isStatic = false);
		virtual ~Game_Object();
	protected:
		// Misc vars
		unsigned int m_ID;

		// Property vars
		LinkedListNode<Game_ObjectProperty>* m_properties;

		// Texture vars
		SDL_Color m_backgroundColor;
		string m_imageTexturePath;
		bool m_needsTextureUpdate;

		// Update vars
		Game_ObjectFUFunc m_frameUpdateFunc;
		Game_ObjectTUFunc m_textureUpdateFunc;

};

class Game_TextObject : public Game_Object
{
	public:
		// Text vars
		double textScaling;
		bool autoSize;

		// Text funcs
		string getText();
		SDL_Color getTextColor();
		void setText(string text);
		void setTextColor(SDL_Color color);

		SDL_Surface* renderText();

		Game_TextObject(int x, int y, int w, int h, bool isStatic = false);
	private:
		// Text vars
		string m_text;
		SDL_Color m_textColor;

};

class Game_AdvancedObject;
typedef void (*Game_ObjectEventFunc)(Game_AdvancedObject& object, SDL_Event& eventData);

class Game_AdvancedObject : public Game_TextObject
{
	public:
		// Event vars
		Game_ObjectEventFunc keyTypedFunc;
		Game_ObjectEventFunc mouseClickedFunc;

		// Event funcs
		bool callEventFunction(Game_ObjectEventType type, SDL_Event& event);
		void setEventFunction(Game_ObjectEventType type, Game_ObjectEventFunc function);

		// Property funcs
		int getIntProperty(string name, int defaultValue);
		bool getBoolProperty(string name, bool defaultValue);
		string getStringProperty(string name, string defaultValue);

		template <typename T>
		void setProperty(string name, T value);

		Game_AdvancedObject(int x, int y, int w, int h, bool isStatic = false);
		~Game_AdvancedObject();
	private:
		// Misc funcs
		LinkedListNode<Game_ObjectProperty>* findPropertyByName(string name);

};

template <typename T>
void Game_AdvancedObject::setProperty(string name, T value)
{
	for (unsigned int i = 0; i < name.length(); i++)
		name[i] = tolower(name.at(i));

	LinkedListNode<Game_ObjectProperty>* propertyNode = findPropertyByName(name);
	if (!propertyNode)
	{
		propertyNode = new LinkedListNode<Game_ObjectProperty>();
		propertyNode->value->name = name;
		propertyNode->nextNode = m_properties;
		m_properties = propertyNode;
	}

	propertyNode->value->setValue(value);
}

#endif
