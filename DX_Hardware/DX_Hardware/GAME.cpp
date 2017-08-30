#include "GAME.h"

GAME::GAME(MENUGPU * menugpu, GAMEGPU * gamegpu)
{
	teamcount = TWO;
	presentteamturn = MENU;

	this->menugpu = menugpu;
	this->menugpu->CreateRenderTargetView();
	this->menugpu->DefineViewPort();
	this->menugpu->DefineAndCreateDepthStencil();

	this->gamegpu = gamegpu;
	this->gamegpu->CreateRenderTargetView();
	this->gamegpu->DefineViewPort();
	this->gamegpu->DefineAndCreateDepthStencil();
}
GAME::~GAME()
{
}

void GAME::Run(XTime &T)
{

	/*switch (presentteamturn)
	{
	case MENU:
	{
		break;
	}
	case ONE:
	{
		menugpu->PlayerInput(menugpu->teamone, ONE, TWO);
		break;
	}
	case TWO:
	{
		menugpu->PlayerInput(menugpu->teamtwo, TWO, ONE);
		break;
	}
	}
	if (menugpu->turnended)
	{
		presentteamturn++;
		if (presentteamturn > teamcount)
			presentteamturn = 1;
		menugpu->turnended = false;
	}
	menugpu->CameraUpdate(T);
	menugpu->DrawToScreen();
*/
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

