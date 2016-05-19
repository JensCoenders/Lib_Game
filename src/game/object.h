#include <string>
#include <SDL.h>

typedef struct game_point
{
		int x, y;
		std::string toString();

} Game_Point;

typedef struct game_rect
{
		int width, height;
		std::string toString();

} Game_Rect;

class Game_Object
{
	public:
		int getID();
		void setWorldSize(Game_Rect size);
		void setWorldCoords(Game_Point coords);

		bool m_needsTextureUpdate;
		SDL_Texture* m_lastRenderedTexture;

		virtual SDL_Texture* compileTexture(SDL_Surface* surface, SDL_Renderer* softwareRenderer) = 0;
		virtual void frameUpdate() = 0;

		Game_Object(int worldX = 0, int worldY = 0, int worldWidth = 0, int worldHeight = 0);
		virtual ~Game_Object() {};
	private:
		int m_ID;
		Game_Point m_worldCoords;
		Game_Rect m_worldSize;
};
