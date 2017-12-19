#include "OBJECT.h"



OBJECT::OBJECT()
{
	positionindex[0] = 0;
	positionindex[1] = 0;
	shipmoveheading = 0;
	shipmoverange = 0;
	shipmovecount = 0;
	shipselectedheading = 0;
	alive = true;
	ship = false;
	quad = false;
	dirty = false;
}


OBJECT::~OBJECT()
{
	mesh.~RENDERABLECOMPONENT();
}

