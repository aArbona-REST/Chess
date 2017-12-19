#pragma once
#include "XTime.h"
#include "OBJECT.h"
#include "RENDERABLECOMPONENT.h"
#include "Save.h"
#include "USERINPUT.h"
#include "DDSTextureLoader.h"
#include "MENUGPU.h"
#include "GAMEGPU.h"
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
	void GAME::SceneTransition();
	void GAME::TurnTransition();
	unsigned int teamcount;
	unsigned int presentteamturn;//change to unsigned char (there will never be more than 255 players as a unsigned char)
	unsigned int presentscene;
	unsigned int serverORclient;
	GAMEGPU * gamegpu;
	MENUGPU * menugpu;
public:
	enum GAMESCENE { MENUSCENE = 1, SERVERSETUPSCENE, CLIENTSETUPSCENE, GAMESCENE, NETWORKGAMESCENE, SHUTDOWN, NETWORKSHUTDOWN };
	enum TEAMLIST { ONE = 1, TWO };
	enum NETWORKCONNECTION { SERVER = 1, CLIENT };
	GAME(HWND &window);
	~GAME();
	void GAME::Run(XTime &T);
};

