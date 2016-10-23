#ifndef GAME_OBJECT_H
#define GAME_OBJECT_H

#include <SDL2/SDL.h>
#include "game_types.h"

/* Function types */

class Game_Object;

typedef void (*Game_ObjectFUFunc)(Game_Object& object);
typedef SDL_Surface* (*Game_ObjectTUFunc)(Game_Object& object, Game_RenderEquipment& equipment);
typedef void (*Game_ObjectEventFunc)(Game_Object& object, Game_ObjectEvent& eventData);

/* Modules */

typedef enum Game_ModuleType
{
	MODULE_COLOR_BACKGROUND = 1,
	MODULE_EVENT = 2,
	MODULE_IMAGE_BACKGROUND = 4,
	MODULE_PROPERTY = 8,
	MODULE_TEXT = 16,
	MODULE_EXTRA_BOUNDS = 32,
	MODULE_ALL = 63

} Game_ModuleType;

typedef struct Game_Module
{
	public:
		Game_Object* getParent();
		Game_Module(Game_Object* parent);
	private:
		Game_Object* m_parent;

} Game_Module;

typedef struct Game_ModuleColorBackground : public Game_Module
{
	public:
		SDL_Color backgroundColor;
		Game_ModuleColorBackground(Game_Object* parent);

} Game_ModuleColorBackground;

typedef struct Game_ModuleEvent : public Game_Module
{
	public:
		Game_ObjectEventFunc keyTypedFunc;
		Game_ObjectEventFunc mouseClickedFunc;

		bool callEventFunction(Game_ObjectEventType type, Game_ObjectEvent& event);
		Game_ModuleEvent(Game_Object* parent);

} Game_ModuleEvent;

typedef struct Game_ModuleImageBackground : public Game_Module
{
	public:
		std::string getTexturePath();
		void setTexturePath(std::string texturePath);

		Game_ModuleImageBackground(Game_Object* parent);
	private:
		std::string m_texturePath;

} Game_ModuleImageBackground;

typedef struct Game_ModuleProperty : public Game_Module
{
	public:
		LinkedListNode<Game_ObjectProperty>* propertyList;

		int getIntProperty(std::string name, int defaultValue);
		bool getBoolProperty(std::string name, bool defaultValue);
		std::string getStringProperty(std::string name, std::string defaultValue);

		template<typename T>
		void setProperty(std::string name, T value);

		Game_ModuleProperty(Game_Object* parent);
		~Game_ModuleProperty();
	private:
		LinkedListNode<Game_ObjectProperty>* findPropertyByName(std::string name);

} Game_ModuleProperty;

typedef struct Game_ModuleText : public Game_Module
{
	public:
		bool autoSize;
		float textScaling;
		SDL_Surface* lastRenderSurface;

		std::string getText();
		void setText(std::string text);
		void setTextSize(int size);
		SDL_Surface* renderText();

		SDL_Color getTextColor();
		void setTextColor(SDL_Color color);

		Game_ModuleText(Game_Object* parent);
	private:
		std::string m_text;
		SDL_Color m_textColor;

} Game_ModuleText;

typedef struct Game_ModuleExtraBounds : public Game_Module
{
	public:
		bool enabled;

		int marginTop;
		int marginRight;
		int marginBottom;
		int marginLeft;
		int paddingTop;
		int paddingRight;
		int paddingBottom;
		int paddingLeft;

		bool fillScreen;
		Game_ObjectFloatMode floatMode;

		Game_ModuleExtraBounds(Game_Object* parent);

} Game_ModuleExtraBounds;

/* Object */

class Game_Object
{
	public:
		// Modules
		Game_ModuleColorBackground* colorBackgroundModule;
		Game_ModuleEvent* eventModule;
		Game_ModuleImageBackground* imageBackgroundModule;
		Game_ModuleProperty* propertyModule;
		Game_ModuleText* textModule;
		Game_ModuleExtraBounds* extraBoundsModule;

		bool isModuleEnabled(Game_ModuleType module);
		void setModuleEnabled(Game_ModuleType module, bool enabled);

		// Update
		SDL_Texture* lastRenderedTexture;
		bool isOutsideCameraBounds;

		void runFrameUpdate();
		void setFrameUpdate(Game_ObjectFUFunc function);
		SDL_Surface* runTextureUpdate(Game_RenderEquipment& equipment);
		void setTextureUpdate(Game_ObjectTUFunc function);

		bool needsTextureUpdate();
		void requestTextureUpdate();
		void satisfyTextureUpdate();

		// World
		Game_Point position;
		Game_Rect size;
		double rotation;

		// Misc
		bool isStatic;
		bool isVisible;
		unsigned int getID();

		Game_Object(int x, int y, int w, int h);
		Game_Object(int x, int y, int w, int h, bool isStatic);
		Game_Object(int x, int y, int w, int h, Game_ModuleType enabledModules);
		Game_Object(int x, int y, int w, int h, bool isStatic, Game_ModuleType enabledModules);
		virtual ~Game_Object();
	protected:
		// Texture
		bool m_needsTextureUpdate;

		// Modules
		int m_enabledModules;

		// Update
		Game_ObjectFUFunc m_frameUpdateFunc;
		Game_ObjectTUFunc m_textureUpdateFunc;

		// Misc
		unsigned int m_ID;

};

/* Misc */

typedef struct Game_Camera
{
	public:
		Game_Point position;
		Game_Rect size;
		Game_Object* centeredObject;

		unsigned char movementDirection;
		int movementSpeed;

} Game_Camera;

typedef struct Game_RenderLayer
{
	public:
		int objectCount;
		LinkedListNode<Game_Object>* objectList;

		Game_RenderLayer();
		~Game_RenderLayer();

} Game_RenderLayer;

template<typename T>
void Game_ModuleProperty::setProperty(std::string name, T value)
{

	LinkedListNode<Game_ObjectProperty>* propertyNode = findPropertyByName(name);
	if (!propertyNode)
	{
		propertyNode = new LinkedListNode<Game_ObjectProperty>();
		propertyNode->value = new Game_ObjectProperty();
		propertyNode->value->name = name;
		propertyNode->nextNode = propertyList;

		propertyList = propertyNode;
	}

	propertyNode->value->setValue(value);
}

#endif
