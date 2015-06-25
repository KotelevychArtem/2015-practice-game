#include "Player.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#define TILE_SIZE 32
struct 
{
	SDL_Texture *wall, *grass, *coin, *player, *others;
}Textures;
class Graphics
{
	SDL_Window *wnd;
	SDL_Renderer *renderer;
public:
	Graphics(){}
	Graphics(uint W, uint H)
	{
		if(SDL_Init(SDL_INIT_EVERYTHING) == -1) return;
		wnd = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, W*TILE_SIZE, H*TILE_SIZE, SDL_WINDOW_SHOWN);
		renderer = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED);
	}

	bool LoadTextures();
	void DrawMap(const Map &map) const;
	void DrawPlayer(const Player &p) const;
	void Flush(){ SDL_RenderClear(renderer); }
	void Update(){ SDL_RenderPresent(renderer); }
};

bool Graphics::LoadTextures()
{
	Textures.grass = IMG_LoadTexture(renderer, "grass.bmp");
	Textures.wall = IMG_LoadTexture(renderer, "wall.bmp");
	Textures.coin = IMG_LoadTexture(renderer, "coin.bmp");
	Textures.player = IMG_LoadTexture(renderer, "player.bmp");
	Textures.others = IMG_LoadTexture(renderer, "bot.bmp");
	if(Textures.coin != NULL && Textures.grass != NULL && Textures.others != NULL && Textures.player != NULL && Textures.wall != NULL) return true;
	else return false;
}

void Graphics::DrawMap(const Map &map) const
{
	SDL_Rect rect;
	rect.w = rect.h = TILE_SIZE;
	for(int i(0); i < map.H; ++i)
		for(int j(0); j < map.W; ++j)
		{
			rect.x = TILE_SIZE*j;
			rect.y = TILE_SIZE*i;
			switch(map.Texture[i][j])
			{
			case 'W':
				SDL_RenderCopy(renderer, Textures.wall, NULL, &rect);
				break;
			case ' ':
			case '.':
			case 'p':
				SDL_RenderCopy(renderer, Textures.grass, NULL, &rect);
				if(map.Texture[i][j] == '.')
				{
					SDL_RenderCopy(renderer, Textures.coin, NULL, &rect);
				}
				break;
			}
		}
}

void Graphics::DrawPlayer(const Player &p) const
{
	SDL_Rect rect;
	rect.w = rect.h = TILE_SIZE;
	rect.x = TILE_SIZE*p.X; rect.y = TILE_SIZE*p.Y;
	if(p.State & isOurs)
		SDL_RenderCopy(renderer, Textures.player, NULL, &rect);
	else
		SDL_RenderCopy(renderer, Textures.others, NULL, &rect);
}
