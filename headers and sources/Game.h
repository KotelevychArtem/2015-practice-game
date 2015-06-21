#include "Server.h"
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#define TILE_SIZE 32

class Game
{
	Server server;
	Player p;
	Map map;
	vector<Player> otherPlayers;
	bool isOn;
	SDL_Window *wnd;
	SDL_Renderer *ren;
	uint visionDistance;
	void DrawWnd();
	bool isVisible(const Player& other);
	struct 
	{
		SDL_Texture *wall, *grass, *coin, *player, *others;
	}Textures;

public:
	Game(Server serv)
	{
		if(SDL_Init(SDL_INIT_EVERYTHING) == -1) return;
		isOn = false;
		server = serv;
		p = server.InitPlayer();
		visionDistance = 10;
		otherPlayers = server.GetOtherPlayersFor(p);
		map = server.GetMap();
		wnd = SDL_CreateWindow("Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, map.W*TILE_SIZE, map.H*TILE_SIZE, SDL_WINDOW_SHOWN);
		ren = SDL_CreateRenderer(wnd, -1, SDL_RENDERER_ACCELERATED);
		Textures.grass = IMG_LoadTexture(ren, "grass.bmp");
		Textures.wall = IMG_LoadTexture(ren, "wall.bmp");
		Textures.coin = IMG_LoadTexture(ren, "coin.bmp");
		Textures.player = IMG_LoadTexture(ren, "player.bmp");
		Textures.others = IMG_LoadTexture(ren, "bot.bmp");
	}
	void Start(){ isOn = true; MainLoop(); }
	void Stop(){ isOn = false; }
	void MainLoop();
	bool tryMove(char dir);
};

void Game::MainLoop()
{
	SDL_Event *mainEvant = new SDL_Event();
	SDL_Keysym *keyEvant = new SDL_Keysym();
	Response r;
	while(isOn && mainEvant->type != SDL_QUIT)
	{
		SDL_PollEvent(mainEvant);
		if(mainEvant->type == SDL_KEYDOWN)
		{
			switch(mainEvant->key.keysym.sym)
			{
			case SDLK_UP:
				if(tryMove('N')) server.UpdateMe(p.getId(), 'N');
				break;
			case SDLK_DOWN:
				if(tryMove('S')) server.UpdateMe(p.getId(), 'S');
				break;
			case SDLK_LEFT:
				if(tryMove('W')) server.UpdateMe(p.getId(), 'W');
				break;
			case SDLK_RIGHT:
				if(tryMove('E')) server.UpdateMe(p.getId(), 'E');
			default:
				break;
			}
		}
		server.UpdateAll();
		r = server.getState(p.getId());
		map = r.currentMap;
		p = r.player;
		otherPlayers = r.others;
		DrawWnd();
		SDL_Delay(server.getTick());
	}
}

void Game::DrawWnd()
{
	SDL_RenderClear(ren);
	SDL_Rect rect;
	rect.w = rect.h = 32;
	for(int i(0); i < map.H; ++i)
		for(int j(0); j < map.W; ++j)
		{
			rect.x = 32*j;
			rect.y = 32*i;
			switch(map.Texture[i][j])
			{
			case 'W':
				SDL_RenderCopy(ren, Textures.wall, NULL, &rect);
				break;
			case ' ':
			case '.':
				SDL_RenderCopy(ren, Textures.grass, NULL, &rect);
				if(map.Texture[i][j] == '.')
				{
					SDL_RenderCopy(ren, Textures.coin, NULL, &rect);
				}
				break;
			}
		}
	for(Player player : otherPlayers)
	{
		if(isVisible(player))
		{
			rect.x = 32*player.X; rect.y = 32*player.Y;
			SDL_RenderCopy(ren, Textures.others, NULL, &rect);
		}
	}
	rect.x = 32*p.X; rect.y = 32*p.Y;
	SDL_RenderCopy(ren, Textures.player, NULL, &rect);
	SDL_RenderPresent(ren);
}

bool Game::tryMove(char dir)
{
	switch(dir)
	{
		case 'n':
		case 'N':
			if(map.Texture[p.Y-1][p.X] != 'W') return true;
			break;
		case 's':
		case 'S':
			if(map.Texture[p.Y+1][p.X] != 'W') return true; 
			break;
		case 'w':
		case 'W':
			if(map.Texture[p.Y][p.X-1] != 'W') return true;
			break;
		case 'e':
		case 'E':
			if(map.Texture[p.Y][p.X+1] != 'W') return true; 
	}
	return false;
}

bool Game::isVisible(const Player& other)
{
	if(p.X != other.X && p.Y != other.Y) return false;
	if(p.X == other.X && std::abs((int)p.Y - (int)other.Y) < visionDistance)
		if(p.Y > other.Y)
		{
			for(int i(other.Y+1); i < p.Y; ++i)
				if(map.Texture[i][p.X] == 'W') return false;
			return true;
		}
		else
		{
			for(int i(p.Y+1); i < other.Y; ++i)
				if(map.Texture[i][p.X] == 'W') return false;
			return true;
		}
	if(p.Y == other.Y && std::abs((int)p.X - (int)other.X) < visionDistance)
		if(p.X > other.X)
		{
			for(int i(other.X+1); i < p.X; ++i)
				if(map.Texture[p.Y][i] == 'W') return false;
			return true;
		}
		else
		{
			for(int i(p.X+1); i < other.X; ++i)
				if(map.Texture[p.Y][i] == 'W') return false;
			return true;
		}
	return false;
}