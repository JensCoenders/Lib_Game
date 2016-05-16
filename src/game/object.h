#include <string>
#include <SDL.h>

typedef struct game_bounds
{
	public:
		int worldX, worldY;
		int worldWidth, worldHeight;

		std::string toString();
} Game_Bounds;

class Game_Object
{
	public:
		// General
		int getID();
		void setBounds(Game_Bounds bounds);

		// Texture
		virtual SDL_Texture* render(SDL_Surface* surface, SDL_Renderer* softwareRenderer);

		Game_Object(int worldX = 0, int worldY = 0, int worldWidth = 0, int worldHeight = 0);
		virtual ~Game_Object();
	private:
		// General
		int m_ID;
		Game_Bounds m_bounds;
};
