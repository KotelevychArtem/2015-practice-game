#pragma once
#include "Player.h"
#include <vector>
#include <map>
#include <WinSock2.h>

using namespace std;
typedef map<uint, pair<Player, char> > Players;
struct Response
	{
		Map currentMap;
		Player player;
		vector<Player> others;
		Response(){}
		Response(Map m, Player p, vector<Player> &oth){ currentMap = m; player = p; others = oth; }
	};

class Server
{
	Map map;
	Players players;
	uint RefreshTime;
	void Collision(uint id);
	void Collide(uint p1, uint p2);
	uint FindPlayer(uint x, uint y);
	pair<uint, uint> FindPlace();
	vector<pair<uint, int> > bots;
	bool botEnable;
	bool started;
	
	uint generateId()
	{
		uint id = rand();
		while(players.find(id) != players.end())
		{
			id = rand();
		}
		return id;
	}
public:
	Server(){}
	Server(uint mapSizeW, uint mapSizeH, uint refreshTime, uint botsCount = 0)
	{
		RefreshTime = refreshTime;
		map.RecursiveGenerate(mapSizeW, mapSizeH);
		//map.Generate(mapSizeH, mapSizeW);
		if(botsCount)
		{
			botEnable = true;
			for(int i(0); i < botsCount; ++i)
				bots.push_back(make_pair(InitPlayer().getId(), 0));
		}
	}
	Player InitPlayer()
	{
		pair<uint, uint> coords = FindPlace();
		map.Texture[coords.second][coords.first] = 'p';
		Player p(generateId(), coords.first, coords.second, isRUNNER, 0);
		players.insert(std::make_pair(p.getId(), std::make_pair(p, 0)));
		return p;
	}
	Map GetMap(){ return map; }
	uint getTick(){ return RefreshTime; }
	vector<Player> GetOtherPlayersFor(Player p);
	void Update(uint id);
	void moveBots();
	void UpdateMe(uint id, char move);
	Response getState(uint id);
	void UpdateAll();
	void Start(){ started = true; }
	void Stop(){ started = false; }
	bool isStarted(){ return started; }
};

vector<Player> Server::GetOtherPlayersFor(Player p)
{
	uint id = p.getId();
	vector<Player> others(players.size()-1);
	uint i(0);
	for(pair<uint, pair<Player, char> > other : players)
		if(other.first != id){ others[i] = other.second.first; ++i; }
	return others;
}

pair<uint, uint> Server::FindPlace()
{
	uint x, y;
	while(true)
	{
		x = rand()%map.W;
		y = rand()%map.H;
		if(map.Texture[y][x] == ' ') break;
	}
	return std::make_pair(x, y);
}

void Server::Update(uint id)
{
	pair<Player, char> *p = &players[id];
	if(p->first.State & isDEAD) return;
	Collision(id);
	map.Texture[p->first.Y][p->first.X] = ' ';
	switch(p->second)
	{
		case 'n':
		case 'N':
			p->first.Y -= 1;
			if(map.Texture[p->first.Y][p->first.X] == '.')
			{
				++p->first.Score;
				if(p->first.Score > 20) p->first.State = isPREDATOR;
				printf("Player %u score: %u \n", id, players.at(id).first.Score);
			}
			break;
		case 's':
		case 'S':
			p->first.Y += 1;
			if(map.Texture[p->first.Y][p->first.X] == '.') 
			{
				++p->first.Score;
				if(p->first.Score > 20) p->first.State = isPREDATOR;
				printf("Player %u score: %u \n", id, players.at(id).first.Score);
			}
			break;
		case 'w':
		case 'W':
			p->first.X -= 1;
			if(map.Texture[p->first.Y][p->first.X] == '.')
			{
				++p->first.Score;
				if(p->first.Score > 20) p->first.State = isPREDATOR;
				printf("Player %u score: %u \n", id, players.at(id).first.Score);
			}
			break;
		case 'e':
		case 'E':
			players.at(id).first.X += 1;
			if(map.Texture[p->first.Y][p->first.X] == '.')
			{
				++p->first.Score;
				if(p->first.Score > 20) p->first.State = isPREDATOR;
				printf("Player %u score: %u \n", id, players.at(id).first.Score);
			}
		default:
			break;
	}
	map.Texture[p->first.Y][p->first.X] = 'p';
	/*Response r(map, players.at(id).first, GetOtherPlayersFor(players.at(id).first));
	return r;*/
}

void Server::moveBots()
{
	const char t[] = {'N', 'E', 'S', 'W'};
	for(pair<uint, int> &bot : bots)
	{
		int d = bot.second;
		Player findedBot = players.at(bot.first).first;
		if(map.Texture[findedBot.Y - 1][findedBot.X] == '.'){ d = 0; goto skipRand; }
		else if(map.Texture[findedBot.Y][findedBot.X + 1] == '.'){ d = 1; goto skipRand; }
		else if(map.Texture[findedBot.Y + 1][findedBot.X] == '.'){ d = 2; goto skipRand; }
		else if(map.Texture[findedBot.Y][findedBot.X - 1] == '.'){ d = 3; goto skipRand; }
		if(d > 3) d = 0;
		else if(d < 0) d = 3;
		int ran = rand()%35;
		if(ran < 4) d = ran;
	skipRand:
		switch(t[d])
		{
			case 'n':
			case 'N':
				if(map.Texture[findedBot.Y-1][findedBot.X] != 'W') UpdateMe(bot.first, t[d]);
				else rand()&1 ? ++d : --d;
				break;
			case 's':
			case 'S':
				if(map.Texture[findedBot.Y+1][findedBot.X] != 'W') UpdateMe(bot.first, t[d]); 
				else rand()&1 ? ++d : --d;
				break;
			case 'w':
			case 'W':
				if(map.Texture[findedBot.Y][findedBot.X-1] != 'W') UpdateMe(bot.first, t[d]);
				else  rand()&1 ? ++d : --d;
				break;
			case 'e':
			case 'E':
				if(map.Texture[findedBot.Y][findedBot.X+1] != 'W') UpdateMe(bot.first, t[d]);
				else rand()&1 ? ++d : --d;
		}
		bot.second = d;
	}
}

void Server::UpdateMe(uint id, char move)
{
	players.at(id).second = move;
}

Response Server::getState(uint id)
{
	Response r(map, players.at(id).first, GetOtherPlayersFor(players.at(id).first));
	return r;
}

void Server::UpdateAll()
{
	moveBots();
	for(pair<uint, pair<Player, char> > player : players)
	{
		Update(player.first);
		players.at(player.second.first.getId()).second = 0;
	}
}

void Server::Collision(uint id)
{
	uint id2;
	pair<Player, char> *p = &players[id];
	switch(p->second)
	{
	case 'n':
	case 'N':
		if(map.Texture[p->first.Y - 1][p->first.X] == 'p') { id2 = FindPlayer(p->first.X, p->first.Y - 1); if(players[id2].second == 'S' || players[id2].second == 0){ Collide(id, id2); return; } }
		else if(p->first.Y - 2 > 0 && map.Texture[p->first.Y - 2][p->first.X] == 'p'){id2 = FindPlayer(p->first.X, p->first.Y - 2); if(players[id2].second == 'S'){ Collide(id, id2); return; } }
		else if(map.Texture[p->first.Y - 1][p->first.X - 1] == 'p'){id2 = FindPlayer(p->first.X - 1, p->first.Y - 1); if(players[id2].second == 'E'){ Collide(id, id2); return; } }
		else if(map.Texture[p->first.Y - 1][p->first.X + 1] == 'p'){id2 = FindPlayer(p->first.X + 1, p->first.Y - 1); if(players[id2].second == 'W'){ Collide(id, id2); return; } }
		break;
	case 's':
	case 'S':
		if(map.Texture[p->first.Y + 1][p->first.X] == 'p'){ id2 = FindPlayer(p->first.X, p->first.Y + 1); if(players[id2].second == 'N' || players[id2].second == 0){ Collide(id, id2); return; } }
		else if(p->first.Y + 2 < map.H && map.Texture[p->first.Y + 2][p->first.X] == 'p'){id2 = FindPlayer(p->first.X, p->first.Y + 2); if(players[id2].second == 'N'){ Collide(id, id2); return; } }
		else if( map.Texture[p->first.Y + 1][p->first.X - 1] == 'p'){id2 = FindPlayer(p->first.X - 1, p->first.Y + 1); if(players[id2].second == 'E'){ Collide(id, id2); return; } }
		else if(map.Texture[p->first.Y + 1][p->first.X + 1] == 'p'){id2 = FindPlayer(p->first.X + 1, p->first.Y + 1); if(players[id2].second == 'W'){ Collide(id, id2); return; } }
		break;
	case 'w':
	case 'W':
		if(map.Texture[p->first.Y][p->first.X - 1] == 'p'){ id2 = FindPlayer(p->first.X - 1, p->first.Y); if(players[id2].second == 'E' || players[id2].second == 0){ Collide(id, id2); return; } }
		else if(p->first.X - 2 > 0 && map.Texture[p->first.Y][p->first.X - 2] == 'p'){id2 = FindPlayer(p->first.X - 2, p->first.Y); if(players[id2].second == 'E'){ Collide(id, id2); return; } }
		else if(map.Texture[p->first.Y + 1][p->first.X - 1] == 'p'){id2 = FindPlayer(p->first.X - 1, p->first.Y + 1); if(players[id2].second == 'N'){ Collide(id, id2); return; } }
		else if(map.Texture[p->first.Y - 1][p->first.X - 1] == 'p'){id2 = FindPlayer(p->first.X - 1, p->first.Y - 1); if(players[id2].second == 'S'){ Collide(id, id2); return; } }
		break;
	case 'e':
	case 'E':
		if(map.Texture[p->first.Y][p->first.X + 1] == 'p'){ id2 = FindPlayer(p->first.X + 1, p->first.Y); if(players[id2].second == 'W' || players[id2].second == 0){ Collide(id, id2); return; } }
		else if(p->first.X + 2 < map.W && map.Texture[p->first.Y][p->first.X + 2] == 'p'){id2 = FindPlayer(p->first.X + 2, p->first.Y); if(players[id2].second == 'W'){ Collide(id, id2); return; } }
		else if(map.Texture[p->first.Y + 1][p->first.X + 1] == 'p'){id2 = FindPlayer(p->first.X + 1, p->first.Y + 1); if(players[id2].second == 'N'){ Collide(id, id2); return; } }
		else if(map.Texture[p->first.Y - 1][p->first.X + 1] == 'p'){id2 = FindPlayer(p->first.X + 1, p->first.Y - 1); if(players[id2].second == 'S'){ Collide(id, id2); return; } }
		break;
	default:
		break;
	}
}

uint Server::FindPlayer(uint x, uint y)
{
	for(pair<uint, pair<Player, char> > t : players)
		if(t.second.first.X == x && t.second.first.Y == y) return t.first;
	return 0;
}

void Server::Collide(uint id1, uint id2)
{
	if(!(id1 && id2)) return;
	Player *p1 = &players[id1].first, *p2 = &players[id2].first;
	char res = p1->State | p2->State;
	if(!((p1->State | p2->State) & isPREDATOR)) return;
	if(p1->State & p2->State & isPREDATOR){ p1->Score > p2->Score ? p2->State = isDEAD : p1->State = isDEAD; return; }
	p1->State & isPREDATOR ? p2->State = isDEAD : p1->State = isDEAD;
}