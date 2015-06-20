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
	void Collision(Player &other);
	pair<uint, uint> FindPlace();
	vector<pair<uint, int> > bots;
	bool botEnable;
	
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
		//map.RecursiveGenerate(0, 0, mapSizeH, mapSizeW);
		map.Generate(mapSizeH, mapSizeW);
		if(botsCount)
		{
			botEnable = true;
			for(int i(0); i < botsCount; ++i)
				bots.push_back(make_pair(InitPlayer().getId(), 0));
		}
	}
	Player InitPlayer()
	{
		pair<uint, uint> cords = FindPlace();
		Player p(generateId(), cords.first, cords.second, isRUNNER, 0);
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
	map.Texture[players.at(id).first.Y][players.at(id).first.X] = ' ';
	switch(players.at(id).second)
	{
		case 'n':
		case 'N':
			players.at(id).first.Y -= 1; //shoul to check collisions whith others
			if(map.Texture[players.at(id).first.Y][players.at(id).first.X] == '.')
			{
				++players.at(id).first.Score;
				printf("Players score: %u \n",players.at(id).first.Score);
				map.Texture[players.at(id).first.Y][players.at(id).first.X] = ' ';
			}
			break;
		case 's':
		case 'S':
			players.at(id).first.Y += 1;
			if(map.Texture[players.at(id).first.Y][players.at(id).first.X] == '.') 
			{
				++players.at(id).first.Score;
				printf("Players score: %u \n",players.at(id).first.Score);
				map.Texture[players.at(id).first.Y][players.at(id).first.X] = ' ';
			}
			break;
		case 'w':
		case 'W':
			players.at(id).first.X -= 1;
			if(map.Texture[players.at(id).first.Y][players.at(id).first.X] == '.')
			{
				++players.at(id).first.Score;
				printf("Players score: %u \n",players.at(id).first.Score);
				map.Texture[players.at(id).first.Y][players.at(id).first.X] = ' ';
			}
			break;
		case 'e':
		case 'E':
			players.at(id).first.X += 1;
			if(map.Texture[players.at(id).first.Y][players.at(id).first.X] == '.')
			{
				++players.at(id).first.Score;
				printf("Players score: %u \n",players.at(id).first.Score);
				map.Texture[players.at(id).first.Y][players.at(id).first.X] = ' ';
			}
		default:
			return;
	}
	/*Response r(map, players.at(id).first, GetOtherPlayersFor(players.at(id).first));
	return r;*/
}

void Server::moveBots()
{
	const char t[] = {'N', 'E', 'S', 'W'};
	for(pair<uint, int> &bot : bots)
	{
		int d = bot.second;
		if(d > 3) d = 0;
		else if(d < 0) d = 3;
		int ran = rand()%200;
		if(ran < 4) d = ran;
		Player findedBot = players.at(bot.first).first;
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