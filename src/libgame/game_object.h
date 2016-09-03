#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include "game_types.h"

/* Type definitions */

class Game_Object;

typedef struct Game_RenderLayer
{
	public:
		int objectCount;
		LinkedListNode<Game_Object>* objectList;

		Game_RenderLayer();
		~Game_RenderLayer();

} Game_RenderLayer;

typedef void (*Game_ObjectFUFunc)(Game_Object& object);
typedef SDL_Surface* (*Game_ObjectTUFunc)(Game_Object& object, Game_RenderEquipment* equipment);

typedef class Game_Object
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

		// Render funcs
		Game_RenderPars* getRenderPars();
		void setRenderPars(Game_RenderPars* renderPars);

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

		// Render vars
		Game_RenderPars* m_renderPars;

		// Texture vars
		bool m_needsTextureUpdate;

		// Update vars
		Game_ObjectFUFunc m_frameUpdateFunc;
		Game_ObjectTUFunc m_textureUpdateFunc;

} Game_Object;

typedef class Game_TextObject : public Game_Object
{
	public:
		// Text vars
		double textScaling;
		bool autoSize;

		// Text funcs
		std::string getText();
		SDL_Color getTextColor();
		void setText(std::string text);
		void setTextColor(SDL_Color color);

		SDL_Surface* renderText();

		Game_TextObject(int x, int y, int w, int h, bool isStatic = false);
	protected:
		// Text vars
		std::string m_text;
		SDL_Color m_textColor;

} Game_TextObject;

class Game_AdvancedObject;
typedef void (*Game_ObjectEventFunc)(Game_AdvancedObject& object, Game_ObjectEvent& eventData);

typedef class Game_AdvancedObject : public Game_TextObject
{
	public:
		// Event vars
		Game_ObjectEventFunc keyTypedFunc;
		Game_ObjectEventFunc mouseClickedFunc;

		// Event funcs
		bool callEventFunction(Game_ObjectEventType type, Game_ObjectEvent& event);

		// Property funcs
		int getIntProperty(std::string name, int defaultValue);
		bool getBoolProperty(std::string name, bool defaultValue);
		std::string getStringProperty(std::string name, std::string defaultValue);

		template<typename T>
		void setProperty(std::string name, T value);

		Game_AdvancedObject(int x, int y, int w, int h, bool isStatic = false);
		~Game_AdvancedObject();
	protected:
		// Misc funcs
		LinkedListNode<Game_ObjectProperty>* findPropertyByName(std::string name);

} Game_AdvancedObject;

template<typename T>
void Game_AdvancedObject::setProperty(std::string name, T value)
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
