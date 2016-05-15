#include <SDL.h>

class Game_Object
{
	public:
		int getID();
		virtual bool render(SDL_Surface* surface, SDL_Renderer* softwareRenderer);

		Game_Object();
		virtual ~Game_Object();
	private:
		int m_ID;
};
