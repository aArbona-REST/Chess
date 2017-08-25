#pragma once
#include "RENDERABLECOMPONENT.h"

class OBJECT
{
public:
	RENDERABLECOMPONENT mesh;
	unsigned int positionindex[2];
	bool alive;
	bool ship;
	//TODO: add variables to allow the game to know what movement the ship can have
	enum SHIPMOVEHEADING { FORWARD = 1, ANGLE };
	enum SHIPMOVERANGE { ONE = 1, TWO, THREE, FOUR };
	unsigned int shipmoveheading;//ship moves n, e, s, w, or ne, se, sw, nw
	unsigned int shipmoverange;//most tiles ship can traverse in one turn
	unsigned int shipmovecount;//spaces moved in relation to ship range(used to move )
	


	OBJECT();
	~OBJECT();
};

