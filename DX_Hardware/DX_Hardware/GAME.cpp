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
	T.Signal();
	gpu->Set();
	gpu->Clear();
	gpu->CameraUpdate();//TODO: This function is bugged. The camera does not move.

	switch (presentteamturn)
	{
	case GAME::ONE:
	{
		gpu->PlayerInput(gpu->teamone, ONE);
		break;
	}
	case GAME::TWO:
	{
		gpu->PlayerInput(gpu->teamtwo, TWO);
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

	gpu->Render(T);

}

