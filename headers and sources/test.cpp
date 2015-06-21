#include "Game.h"
#include <iostream>

int main(int argc, char *argv[])
{
	Server serv(20, 35, 100, 10);
	Game game(serv);
	game.Start();
	return 0;
}