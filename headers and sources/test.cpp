#include "Game.h"
#include <iostream>
#include <thread>

void StartServer(Server *serv)
{
	serv->Start();
	while(serv->isStarted())
	{
		serv->UpdateAll();
		Sleep(serv->getTick());
	}
}

int main(int argc, char *argv[])
{
	Server serv(20, 35, 100, 2);
	thread serverThread(StartServer, &serv);
	Game game(&serv);
	game.Start();
	if(serverThread.joinable())
		serverThread.join();
	return 0;
}