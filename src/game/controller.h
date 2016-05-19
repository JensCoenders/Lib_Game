#include <string>
#include <SDL.h>

#include "object.h"

#define GAME_SUCCESS 				0
#define GAME_ERR_ALREADY_INIT		1
#define GAME_CRIT_SDL 				-1
#define GAME_CRIT_SDL_IMG			-2

#define GAME_LAYER_AMOUNT			7

typedef struct game_errormsg
{
	public:
		int errorCode;
		std::string message;
} Game_ErrorMsg;

typedef enum game_eventfunction
{
	EVENT_FUNCTION_NONE = 0,
	EVENT_FUNCTION_KEYBOARD = 1,
	EVENT_FUNCTION_MOUSE = 2,
	EVENT_FUNCTION_WINDOW = 4
} Game_EventFunction;

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
		bool m_running;
		void gameLoop();

		void addObject(Game_Object* object, Game_EventFunction function = EVENT_FUNCTION_NONE);
		void removeObject(Game_Object* object);
		void removeObject(int objectID);

		Game_ErrorMsg initializeSDL(std::string windowTitle);
		void destroySDL();

		Game_Controller();
		~Game_Controller();
	private:
		void processKeyboardEvent(SDL_Event* event);
		void processMouseEvent(SDL_Event* event);
		void processWindowEvent(SDL_Event* event);

		Game_Layer* m_layers;

		bool m_SDLInitialized;
		SDL_Window* m_window;
		SDL_Renderer* m_windowRenderer;
		float m_zoomScale;
};
