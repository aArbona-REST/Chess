#pragma once
#include "OBJECT.h"
class PLAYER
{
public:
	PLAYER();
	~PLAYER();
	unsigned int Team = 0;
	OBJECT objects[18];
	unsigned int PieceCount = 18;
	OBJECT selectedobjecticon;
};

