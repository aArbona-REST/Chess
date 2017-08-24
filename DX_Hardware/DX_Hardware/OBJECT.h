#pragma once
#include "RENDERABLECOMPONENT.h"

class OBJECT
{
public:
	RENDERABLECOMPONENT mesh;
	unsigned int positionindex[2];
	bool alive;
	//TODO: add variables to allow the game to know what ship this object is
	//enum with all ships
	//unsigned int with the actuall ship type
	
	


	OBJECT();
	~OBJECT();
};

