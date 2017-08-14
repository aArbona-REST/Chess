#pragma once
#include "RENDERABLECOMPONENT.h"

class OBJECT
{
public:
	RENDERABLECOMPONENT mesh;
	unsigned int positionindex[2];
	bool alive;

	OBJECT();
	~OBJECT();
};

