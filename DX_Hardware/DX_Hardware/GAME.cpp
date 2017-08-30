#include "GAME.h"

GAME::GAME(HWND &window)
{
	teamcount = TWO;
	presentteamturn = MENU;

	this->menugpu = new MENUGPU(window);
	this->menugpu->CreateRenderTargetView();
	this->menugpu->DefineViewPort();
	this->menugpu->DefineAndCreateDepthStencil();

	this->gamegpu = new GAMEGPU(window);
	this->gamegpu->CreateRenderTargetView();
	this->gamegpu->DefineViewPort();
	this->gamegpu->DefineAndCreateDepthStencil();
}
GAME::~GAME()
{
	delete menugpu;
	delete gamegpu;
}

void GAME::Run(XTime &T)
{

	switch (presentteamturn)
	{
	case MENU:
	{
		menugpu->CameraUpdate(T);
		menugpu->DrawToScreen();
		if (menugpu->loadgame)
		{
			presentteamturn = ONE;
			menugpu->loadgame = false;
		}
		break;
	}
	case ONE:
	{
		gamegpu->PlayerInput(gamegpu->teamone, ONE);
		if (gamegpu->returntomenu)
		{
			presentteamturn = MENU;
			gamegpu->returntomenu = false;
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
		break;
	}
	case TWO:
	{
		gamegpu->PlayerInput(gamegpu->teamtwo, TWO);
		if (gamegpu->returntomenu)
		{
			presentteamturn = MENU;
			gamegpu->returntomenu = false;
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
		break;
	}
	}


}

