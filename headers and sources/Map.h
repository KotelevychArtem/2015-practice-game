#include "common.h"
#include <time.h>
#include <vector>

struct Map
{
	uint H, W;
	char **Texture;
	void LoadFromFile(const char* name);
	void Generate(uint mapH, uint mapW);
	void GenerateEller(uint mapH, uint mapW);
	void RecursiveGenerate(uint a, uint b, uint c, uint d);
};

void Map::LoadFromFile(const char* name)
	{
		FILE *textureStream;
		if((textureStream = fopen(name, "r")) == NULL)
		{
			printf("File not found");
			return;
		}
		fscanf(textureStream, "%lu", &H);
		fscanf(textureStream, "%lu", &W);
		Texture = new char* [H];
		for(int i(0); i < H; ++i)
			Texture[i] = new char[W];
		char t;
		for(int i(0); i < H; ++i)
			for(int j(0); j < W; ++j)
			{
				fscanf(textureStream, "%c", &t);
				if(t != '\n' && t != '\r') Texture[i][j] = t;
				else --j;
			}
	}

void Map::Generate(uint mapW, uint mapH)
{
	H = mapH; W = mapW;
	Texture = new char* [H];
	for(int i(0); i < H; ++i)
		Texture[i] = new char[W];
	for(int i(0); i < H; ++i) Texture[i][0] = Texture[i][W-1] = 'W';
	for(int j(0); j < W; ++j) Texture[0][j] = Texture[H-1][j] = 'W';
	srand(time(0));
	int r;
	for(int i(1); i < H-1; ++i)
		for(int j(1); j < W-1; ++j)
		{
			r = rand();
			if(r & 1)
			{
				if(r & 2) Texture[i][j] = '.';
				else Texture[i][j] = 'W';
			}
			else Texture[i][j] = ' ';
		}
}

void Map::GenerateEller(uint mapH, uint mapW)
{
	H = mapH; W = mapW;
	if(H&1) ++H;
	uint tW = W-2;
	int *t = new int[tW];
	for(int i(0); i < tW; ++i) t[i] = i+1;
	Texture = new char* [H];
	for(int i(0); i < H; ++i)
		Texture[i] = new char[W];
	for(int i(0); i < H; ++i)
		for(int j(0); j < W; ++j)
			Texture[i][j] = 'W';
	srand(time(0));
	for(int j(1); j < tW; ++j)
		if(rand()&1) t[j] = t[j-1];
	for(int i(1); i < H; i += 2)
	{
		for(int j(0); j < tW - 1; ++j)
			if(t[j] != t[j+1]) Texture[i][j+1] = 'W';
			else Texture[i][j+1] = ' ';
		int ind(1);
		for(int j(0); j < tW - 1; ++j)
		{
			ind = 1;
			while(t[j] == t[j+1] && j < tW-1){ ++ind; ++j; }
			Texture[i+1][j - rand()%ind] = ' ';
		}
		for(int j(0); j < tW; ++j)
			if(Texture[i+1][j+1] == 'W') t[j] = j+1;
	}
}

void Map::RecursiveGenerate(uint a, uint b, uint c, uint d)
{
	if(d - b < 2 || c - a < 2) return;
	static bool fill = false;
	if(!fill)
	{
		H = c; W = d;
		if(H&1) ++H;
		uint tW = W-2;
		int *t = new int[tW];
		for(int i(0); i < tW; ++i) t[i] = i+1;
		Texture = new char* [H];
		for(int i(0); i < H; ++i)
			Texture[i] = new char[W];
		for(int i(0); i < H; ++i)
			for(int j(0); j < W; ++j)
				Texture[i][j] = 'W';
		fill = true;
		RecursiveGenerate(1, 1, c-1, d-1);
		return;
	}
	std::pair<uint, uint> center;//, centerT, centerR, centerB, centerL;
	srand(time(0));
	center.first = (a+c)>>1;
	center.second = (b+d)>>1;
	for(int i(0); i <= center.second>>1; ++i)Texture[center.first][b + rand()%(d-b)] = ' ';
	for(int i(0); i <= center.first>>1; ++i)Texture[a + rand()%(c-a)][center.second] = ' ';
	RecursiveGenerate(a, b, center.first, center.second);
	RecursiveGenerate(a, center.second + 1, center.first, d);
	RecursiveGenerate(center.first + 1, b, c, center.second);
	RecursiveGenerate(center.first + 1, center.second + 1, c, d);
}