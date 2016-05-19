#include <iostream>
#include <string>
#include <SDL.h>
#include <typeinfo>

#include "object.h"

#define GAME_SUCCESS 				0
#define GAME_ERR_ALREADY_INIT		1
#define GAME_CRIT_SDL 				-1
#define GAME_CRIT_SDL_IMG			-2

#define GAME_LAYER_AMOUNT			7

using namespace std;

typedef enum game_controllerproperty
{
	PROPERTY_RUNNING,
	PROPERTY_USE_FPS_COUNTER,
	PROPERTY_ZOOMSCALE

} Game_ControllerProperty;

typedef struct game_controllerprops
{
	public:
		bool running;
		bool useFPSCounter;
		float zoomScale;

} Game_ControllerProperties;

typedef struct game_result
{
	public:
		int returnCode;
		std::string message;

} Game_Result;

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

typedef struct game_layer
{
	public:
		int objectCount;
		Game_ObjectNode* objectList;

		game_layer();
		~game_layer();

} Game_Layer;

class Game_Controller
{
	public:
		template<typename T>
		void setProperty(Game_ControllerProperty property, T object);
		template<typename T>
		void setProperty(Game_ControllerProperty property, T* object);

		template<typename T>
		T getProperty(Game_ControllerProperty property);
		template<typename T>
		T* getProperty(Game_ControllerProperty property);

		void gameLoop();

		void addObject(Game_Object* object);
		void removeObject(Game_Object* object);
		void removeObject(int objectID);

		Game_Result initializeSDL(std::string windowTitle);
		void destroySDL();

		Game_Controller();
		~Game_Controller();
	private:
		void processKeyboardEvent(SDL_Event* event);
		void processMouseEvent(SDL_Event* event);
		void processWindowEvent(SDL_Event* event);

		void throwPropertyError(string propertyName, string typeName);
		Game_ControllerProperties m_properties;
		Game_Layer* m_layers;

		bool m_SDLInitialized;
		SDL_Window* m_window;
		SDL_Renderer* m_windowRenderer;
};

template<typename T>
void Game_Controller::setProperty(Game_ControllerProperty property, T object)
{
	setProperty(property, &object);
}

template<typename T>
void Game_Controller::setProperty(Game_ControllerProperty property, T* object)
{
	switch (property)
	{
		case PROPERTY_RUNNING:
			if (typeid(T) != typeid(bool))
			{
				throwPropertyError("PROPERTY_RUNNING", "bool");
				break;
			}
			m_properties.running = (bool) *object;
			break;
		case PROPERTY_USE_FPS_COUNTER:
			if (typeid(T) != typeid(bool))
			{
				throwPropertyError("PROPERTY_USE_FPS_COUNTER", "bool");
				break;
			}
			m_properties.useFPSCounter = (bool) *object;
			break;
		case PROPERTY_ZOOMSCALE:
			if (typeid(T) != typeid(float))
			{
				throwPropertyError("PROPERTY_ZOOMSCALE", "float");
				break;
			}
			m_properties.zoomScale = (float) *object;
			break;
		default:
			cout << "[WARN] Unrecognized property!" << endl;
			break;
	}
}
