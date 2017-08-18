#include "GAME.h"

GAME::GAME(GPU * gpu_handle)
{
	teamcount = 2;
	presentteamturn = 1;
	gpu = gpu_handle;

	gpu->CreateRenderTargetView();
	gpu->DefineViewPort();
	gpu->DefineAndCreateDepthStencil();
}
GAME::~GAME()
{
}

void GAME::Run(XTime &T)
{

	switch (presentteamturn)
	{
	case GAME::ONE:
	{
		gpu->PlayerInput(gpu->teamone, ONE, TWO);
		break;
	}
	case GAME::TWO:
	{
		gpu->PlayerInput(gpu->teamtwo, TWO, ONE);
		break;
	}
	}
	if (gpu->turnended)
	{
		presentteamturn++;
		if (presentteamturn > teamcount)
			presentteamturn = 1;
		gpu->turnended = false;
	}
	gpu->CameraUpdate(T);
	gpu->DrawToScreen();

}

