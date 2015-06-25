#pragma once
#include "Map.h"

enum{isDOUBLE_SPEED = 1, isPREDATOR = 2, isDEAD = 4, isRUNNER = 8, isSET = 16, isOurs = 32};
class Player
{
	uint Id;
public:
	uint X, Y;
	uint Score;
	char State;
	Player(){}
	Player(uint newId, uint newX, uint newY, uint newState, uint newScore)
	{
		Id = newId;
		X = newX; Y = newY;
		Score = newScore;
		State = newState;
	}
	uint getId(){ return Id; }
};