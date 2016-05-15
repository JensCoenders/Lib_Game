#include <string>
#include <SDL.h>

#define GAME_SUCCESS 			0
#define GAME_ERR_ALREADY_INIT	1
#define GAME_CRIT_SDL 			-1
#define GAME_CRIT_SDL_IMG		-2

typedef struct game_errormsg
{
	public:
		int errorCode;
		std::string message;
} Game_ErrorMsg;

class Game_Controller
{
	public:
		// SDL
		Game_ErrorMsg initializeSDL(std::string windowTitle);
		void destroySDL();

		// Game flow
		void startGameLoop();
		void stopGameLoop();

		// Objects
		void addObject();		// TODO: Finish function declaration
		void removeObject();	// TODO: Finish function declaration

		// Instance
		~Game_Controller();
		static Game_Controller* Create();
	private:
		// SDL
		bool m_SDLInitialized;
		SDL_Window* m_window;
		SDL_Renderer* m_windowRenderer;

		// Instance
		Game_Controller();
};
