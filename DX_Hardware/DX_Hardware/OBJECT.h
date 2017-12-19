#pragma once
#include "RENDERABLECOMPONENT.h"

class OBJECT
{
public:
	RENDERABLECOMPONENT mesh;
	unsigned int positionindex[2];
	bool alive;
	bool ship;
	bool quad;
	bool dirty;// flag to update the mesh position to reflect the position index
	//TODO: add variables to allow the game to know what movement the ship can have

	enum SHIPMOVEHEADING { FORWARD = 1, ANGLE };
	enum SHIPMOVERANGE { ZERO, ONE, TWO, THREE, FOUR, TEN = 10 };
	enum HEADING { NONE, UP, UPRIGHT, RIGHT, DOWNRIGHT, DOWN, DOWNLEFT, LEFT, LEFTUP };
	unsigned int shipmoveheading;//ship moves n, e, s, w, or ne, se, sw, nw
	unsigned int shipmoverange;//most tiles ship can traverse in one turn
	unsigned int shipmovecount;//spaces moved in relation to ship range(used to move )
	unsigned int shipselectedheading;//the heading that player selected to move ship in 


	OBJECT();
	~OBJECT();
};

