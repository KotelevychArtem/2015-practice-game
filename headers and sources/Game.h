#pragma once
#include "Graphics.h"
#include "Server.h"
#include <sdkddkver.h>
#include <WinSock2.h>

class Game
{
	Server *server;
	Player p;
	Map map;
	vector<Player> otherPlayers;
	bool isOn;
	SDL_Window *wnd;
	SDL_Renderer *ren;
	Graphics *g;
	uint visionDistance;
	void DrawWnd();
	bool isVisible(const Player& other);
public:
	Game(Server *serv)
	{
		isOn = false;
		server = serv;
		map = server->GetMap();
		p = server->InitPlayer();
		otherPlayers = server->GetOtherPlayersFor(p);
		visionDistance = 10;
		g = new Graphics(map.W, map.H);
		if(!(g->LoadTextures())) return;
	}
	void UpdateData()
	{
		Response r;
		r = server->getState(p.getId());
		map = r.currentMap;
		p = r.player;
		p.State = p.State | isOurs;
		otherPlayers = r.others;
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
		UpdateData();
		SDL_PollEvent(mainEvant);
		if(mainEvant->type == SDL_KEYDOWN && !(p.State & isDEAD))
		{
			
			switch(mainEvant->key.keysym.sym)
			{
			case SDLK_UP:
				if(tryMove('N')) server->UpdateMe(p.getId(), 'N');
				break;
			case SDLK_DOWN:
				if(tryMove('S')) server->UpdateMe(p.getId(), 'S');
				break;
			case SDLK_LEFT:
				if(tryMove('W')) server->UpdateMe(p.getId(), 'W');
				break;
			case SDLK_RIGHT:
				if(tryMove('E')) server->UpdateMe(p.getId(), 'E');
			default:
				break;
			}
		}
		//server->UpdateAll();
		UpdateData();
		DrawWnd();
		SDL_Delay(80);
	}
	server->Stop();
}

void Game::DrawWnd()
{
	g->Flush();
	g->DrawMap(map);
	for(const Player &player : otherPlayers)
		if(!(player.State & isDEAD))
			g->DrawPlayer(player);
	if(!(p.State & isDEAD)) g->DrawPlayer(p);
	g->Update();
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