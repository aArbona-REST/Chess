#pragma once
#include "XTime.h"
#include "OBJECT.h"
#include "RENDERABLECOMPONENT.h"
#include "Save.h"
#include "USERINPUT.h"
#include "DDSTextureLoader.h"
#include "GPU.h"
#include "PLAYER.h"

//#include "Trivial_PS.csh"
//#include "Trivial_VS.csh"
//#include "BasicShader.csh"
//#include "BasicPixelShader.csh"

#include <d3d11.h>
#include <DirectXMath.h>

#pragma comment(lib,"d3d11.lib")
extern USERINPUT input;
extern Save save;
extern int BACKBUFFER_WIDTH;
extern int BACKBUFFER_HEIGHT;

class GAME
{
	enum TEAMLIST	{ONE = 1, TWO};
	unsigned int teamcount;
	unsigned int presentteamturn;

	GPU * gpu;

public:
	GAME(GPU * gpu_handle);
	~GAME();
	void GAME::Run(XTime &T);
};

