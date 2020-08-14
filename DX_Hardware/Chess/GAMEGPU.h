#pragma once
#include "XTime.h"
#include "OBJECT.h"
#include "RENDERABLECOMPONENT.h"
#include "Save.h"
#include "GameVertexShader.csh"
#include "GamePixelShader.csh"
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

class GAMEGPU//TODO:: all the variables are being set here and should be set in the constructor
{
public:
	GAMEGPU(HWND &window);
	~GAMEGPU();
	void DrawToScreen(unsigned int playerteam);
	void ShutDown();
	void PlayerInput(unsigned int playerteam);
	void InitalizePlayerShips(unsigned int team, OBJECT * object, const char * mesh, const wchar_t * texture);
	void InitalizeobjAsset(OBJECT * object, const char * mesh, const wchar_t * texture);
	void InitalizeQuad(OBJECT * object, const wchar_t * texture);
	void AllocateBuffer(OBJECT * object, wchar_t * texture);
	void CreateConstantBuffer(void * data, unsigned int size, ID3D11Buffer * buffer);//bugged function and does not work. 
	void CameraUpdate(XTime &Time);
	void Render(OBJECT * object, unsigned int count);
	void RenderExact(OBJECT * object, unsigned int count);
	void CreateRenderTargetView();
	void DefineViewPort();
	void DefineAndCreateDepthStencil();
private:
	void LoadTitan(unsigned int team, OBJECT * object);
	void LoadFrigate(unsigned int team, OBJECT * object);
	void LoadFighter(unsigned int team, OBJECT * object);

	void Set();
	void Clear();
	float color[4]{ 0.2f, 0.2f, 0.5f, 0.0f };
public:
	HRESULT							HR;
	bool returntomenu = false;
	bool turnended = false;
	OBJECT team[2][18];
	ID3D11Device *device = NULL;//delete the new momory
	ID3D11DeviceContext * context = NULL;//delete
	IDXGISwapChain *swapchain = NULL;//delete
	ID3D11RenderTargetView * rtv = NULL;//delete
	D3D11_VIEWPORT viewport;//??delete??
	ID3D11Texture2D * depthStencil = nullptr;//delete
	ID3D11DepthStencilView * depthStencilView = nullptr;//delete
	ID3D11InputLayout * layout = NULL;//delete
	ID3D11Buffer * constBuffer = NULL;//delete
	VRAM send_to_ram;
	XMFLOAT4X4 camera;
private:



	//re organize the way textures are held in mem. 
	//rather than each object have its texture
	//make the obj have a pointer to the one copy of a texture ()
	ID3D11ShaderResourceView * black = nullptr;
	ID3D11ShaderResourceView * white = nullptr;
	ID3D11ShaderResourceView * grey = nullptr;


	size_t row = 14, depth = 8;

	struct NODE
	{
		XMFLOAT4 position{0.0f, 0.0f, 0.0f, 0.0f};
		unsigned int positionstatus = 0;//the team number
		unsigned int occupieindex = 0;//the index of the piece 
		unsigned int positionindex[2]{ 0,0 };//the index in to the 2D array
		NODE * front = nullptr;
		NODE * back = nullptr;
		NODE * left = nullptr;
		NODE * right = nullptr;
	};

	NODE map[14][8];
	OBJECT quadsmap[14][8];
	OBJECT billboard;
	int selectedobject = -1;
	bool pieceselected = false;

	OBJECT selectedobjecticonwhite;
	OBJECT selectedobjecticonblack;
	unsigned int teamcapacity = 18;
	unsigned int piececount = 18;
	D3D11_BUFFER_DESC bufferdescription;
	D3D11_SUBRESOURCE_DATA InitData;

	ID3D11Buffer * groundvertbuffer = NULL;//delete
	ID3D11Buffer * groundindexbuffer = NULL;//delete
	unsigned int groundindexCount = 0;

	ID3D11VertexShader * Gamevertexshader = NULL;//delete
	ID3D11PixelShader * Gamepixelshader = NULL;//delete

	UINT stride = sizeof(SIMPLE_VERTEX);
	UINT offset = 0;
	D3D11_MAPPED_SUBRESOURCE mapResource;
};