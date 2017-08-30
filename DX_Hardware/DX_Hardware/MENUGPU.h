#pragma once
#include "XTime.h"
#include "OBJECT.h"
#include "RENDERABLECOMPONENT.h"
#include "Save.h"
#include "MenuVertexShader.csh"
#include "MenuPixelShader.csh"
#include "DDSTextureLoader.h"
#include "USERINPUT.h"
#include "PLAYER.h"

#include <d3d11.h>
#include <DirectXMath.h>

#pragma comment(lib,"d3d11.lib")

using namespace DirectX;
extern USERINPUT input;
extern Save save;
extern int BACKBUFFER_WIDTH;
extern int BACKBUFFER_HEIGHT;

class MENUGPU
{
public:
	//XTime Time;
	MENUGPU(HWND &window);
	~MENUGPU();
	void DrawToScreen();
	bool ShutDown();
	void MENUGPU::PlayerInput(OBJECT * objects, unsigned int playerteam, unsigned int enemyteam);
	void MENUGPU::InitalizePlayerShips(unsigned int team, OBJECT * object, char * mesh, wchar_t * texture);
	void MENUGPU::InitalizeobjAsset(OBJECT * object, char * mesh, wchar_t * texture);
	void MENUGPU::InitalizeQuad(OBJECT * object, wchar_t * texture);
	void MENUGPU::AllocateBuffer(OBJECT * object, wchar_t * texture);
	void MENUGPU::CreateConstantBuffer(void * data, unsigned int size, ID3D11Buffer * buffer);//bugged function and does not work. 
	void MENUGPU::CameraUpdate(XTime &Time);
	void MENUGPU::Render(OBJECT * object, unsigned int count);
	void MENUGPU::RenderExact(OBJECT * object, unsigned int count);
	void MENUGPU::CreateRenderTargetView();
	void MENUGPU::DefineViewPort();
	void MENUGPU::DefineAndCreateDepthStencil();
private:
	void MENUGPU::LoadTitan(unsigned int team, OBJECT * object);
	void MENUGPU::LoadBattleship(unsigned int team, OBJECT * object);
	void MENUGPU::LoadCruiser(unsigned int team, OBJECT * object);
	void MENUGPU::LoadFrigate(unsigned int team, OBJECT * object);
	void MENUGPU::LoadDestroyer(unsigned int team, OBJECT * object);
	void MENUGPU::LoadFighter(unsigned int team, OBJECT * object);

	void MENUGPU::Set();
	void MENUGPU::Clear();
	float color[4]{ 0.2f, 0.2f, 0.5f, 0.0f };
	HWND * window = nullptr;
public:
	HRESULT							HR;
private:
	size_t row = 14, depth = 8;

	struct NODE
	{
		XMFLOAT4 position{ 0.0f, 0.0f, 0.0f, 0.0f };
		unsigned int positionstatus = 0;//the team number
		unsigned int occupieindex = 0;//the index of the piece 
		unsigned int positionindex[2]{ 0,0 };//the index in to the 2D array
		NODE * front = nullptr;
		NODE * back = nullptr;
		NODE * left = nullptr;
		NODE * right = nullptr;
	};
	struct QUAD
	{
		unsigned int vertcount = 4;
		unsigned int indexcount = 6;
		unsigned int groundPlaneindex[6]{ 0,1,3,1,2,3 };
		XMFLOAT4 groundColor{ 1.0f, 1.0f, 1.0f, 0.0f };
		SIMPLE_VERTEX groundPlane[4]{
			{ XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f) ,XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f) ,groundColor ,XMFLOAT2(0.0f, 1.0f) },
			{ XMFLOAT4(1.0f, 0.0f, -1.0f, 1.0f) ,XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f) ,groundColor ,XMFLOAT2(1.0f, 1.0f) },
			{ XMFLOAT4(-1.0f, 0.0f, -1.0f, 1.0f) ,XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f) ,groundColor ,XMFLOAT2(1.0f, 0.0f) },
			{ XMFLOAT4(-1.0f, 0.0f, 1.0f, 1.0f) ,XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f) ,groundColor ,XMFLOAT2(0.0f, 0.0f) },
		};
	};
	QUAD quad;
	NODE map[14][8];
	OBJECT quadsmap[14][8];
	OBJECT billboard;
	int selectedobject = -1;
	bool pieceselected = false;
public:
	bool turnended = false;
	//OBJECT teamone[18];
	//OBJECT teamtwo[18];
	//int teamonecount = 18;
	//int teamtwocount = 18;
	OBJECT teamone[6];
	OBJECT teamtwo[6];
	int teamonecount = 6;
	int teamtwocount = 6;
private:
	OBJECT selectedobjecticon;
	//modifies to reduce player ships to test a full runthough quicker
	//unsigned int teamcapacity = 18;
	//unsigned int piececount = 18;
	unsigned int teamcapacity = 6;
	unsigned int piececount = 6;
	D3D11_BUFFER_DESC bufferdescription;
	D3D11_SUBRESOURCE_DATA InitData;

public:
	ID3D11Device *device = NULL;
	ID3D11DeviceContext * context = NULL;
	IDXGISwapChain *swapchain = NULL;
	ID3D11RenderTargetView * rtv = NULL;
	D3D11_VIEWPORT viewport;
	ID3D11Texture2D * depthStencil = nullptr;
	ID3D11DepthStencilView * depthStencilView = nullptr;
private:

	ID3D11Buffer * groundvertbuffer = NULL;
	ID3D11Buffer * groundindexbuffer = NULL;
	unsigned int groundindexCount = 0;

	ID3D11VertexShader * Menuvertexshader = NULL;
	ID3D11PixelShader * Menupixelshader = NULL;


public:
	ID3D11InputLayout * layout = NULL;
	ID3D11Buffer * constBuffer = NULL;
	VRAM send_to_ram;
	XMFLOAT4X4 camera;
private:
	UINT stride = sizeof(SIMPLE_VERTEX);
	UINT offset = 0;
	D3D11_MAPPED_SUBRESOURCE mapResource;
};
