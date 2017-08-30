#include "GAME.h"

GAME::GAME(MENUGPU * menugpu, GAMEGPU * gamegpu)
{
	teamcount = TWO;
	presentteamturn = ONE;
	presentscene = GAMESCENE;
	this->menugpu = menugpu;
	this->gamegpu = gamegpu;
	gamegpu->CreateRenderTargetView();
	gamegpu->DefineViewPort();
	gamegpu->DefineAndCreateDepthStencil();
}
GAME::~GAME()
{
}

void GAME::Run(XTime &T)
{

	switch (presentteamturn)
	{
	case MENU:
	{

		break;
	}
	case ONE:
	{
		gamegpu->PlayerInput(gamegpu->teamone, ONE, TWO);
		break;
	}
	case TWO:
	{
		gamegpu->PlayerInput(gamegpu->teamtwo, TWO, ONE);
		break;
	}
	}
	if (gamegpu->turnended)
	{
		presentteamturn++;
		if (presentteamturn > teamcount)
			presentteamturn = 1;
		gamegpu->turnended = false;
	}
	gamegpu->CameraUpdate(T);
	gamegpu->DrawToScreen();

}

