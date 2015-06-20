#include "Game.h"
#include <iostream>

int main(int argc, char *argv[])
{
	Server serv(20, 35, 150, 5);
	Game game(serv);
	game.Start();
	return 0;
}