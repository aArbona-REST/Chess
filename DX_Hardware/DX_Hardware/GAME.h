#pragma once
#include "XTime.h"
#include "OBJECT.h"
#include "RENDERABLECOMPONENT.h"
#include "Save.h"
#include "USERINPUT.h"
#include "DDSTextureLoader.h"
#include "GPU.h"
#include "PLAYER.h"

#include <d3d11.h>
#include <DirectXMath.h>

#pragma comment(lib,"d3d11.lib")
extern USERINPUT input;
extern Save save;
extern int BACKBUFFER_WIDTH;
extern int BACKBUFFER_HEIGHT;

class GAME
{
	unsigned int teamcount;
	unsigned int presentteamturn;

	GPU * gpu;

public:
	enum TEAMLIST	{ONE = 1, TWO};
	GAME(GPU * gpu_handle);
	~GAME();
	void GAME::Run(XTime &T);
};

