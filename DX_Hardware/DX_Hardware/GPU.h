#pragma once
#include "XTime.h"
#include "OBJECT.h"
#include "RENDERABLECOMPONENT.h"
#include "Save.h"
#include "Trivial_PS.csh"
#include "Trivial_VS.csh"
#include "BasicShader.csh"
#include "BasicPixelShader.csh"
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

class GPU
{
public:
	XTime Time;
	GPU(HWND &window);
	~GPU();
	void DrawToScreen(XTime &Time);
	bool ShutDown();
	void GPU::PlayerInput(OBJECT * objects, unsigned int playerteam, unsigned int enemyteam);
	void GPU::LoadPlayerAssets(unsigned int team, OBJECT * object, char * mesh, wchar_t * texture);
	void GPU::LoadobjAsset(OBJECT * object, char * mesh, wchar_t * texture);
	void GPU::LoadtriAsset(OBJECT * object, wchar_t * texture, SIMPLE_VERTEX * verts, unsigned int vertcount, unsigned int * indices, unsigned int indicescount);
	void GPU::AllocateBuffer(OBJECT * object, wchar_t * texture);
	void GPU::CreateConstantBuffer(void * data, unsigned int size, ID3D11Buffer * buffer);//bugged function and does not work. 
	void GPU::CameraUpdate();
	void GPU::Render(OBJECT * object, unsigned int count);
	void GPU::CreateRenderTargetView();
	void GPU::DefineViewPort();
	void GPU::DefineAndCreateDepthStencil();
	void GPU::Set();
	void GPU::Clear();
	float color[4]{ 0.2f, 0.2f, 0.5f, 0.0f };
private:
	HWND * window = nullptr; 
public:
	HRESULT							HR;
private:
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
	unsigned int positionstatus[14][8];
	int selectedobject = -1;
	bool pieceselected = false;
public:
	bool turnended = false;
	OBJECT teamone[18];
	OBJECT teamtwo[18];
private:	
	OBJECT selectedobjecticon;
	unsigned int teamcapacity = 18;
	unsigned int piececount = 18;

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


	ID3D11ShaderResourceView * pModelTexture = nullptr;

	ID3D11Buffer * groundvertbuffer = NULL;
	ID3D11Buffer * groundindexbuffer = NULL;
	unsigned int groundindexCount = 0;


	ID3D11VertexShader * vertexshader = NULL;
	ID3D11PixelShader * pixelshader = NULL;

	ID3D11VertexShader * Basicvertexshader = NULL;
	ID3D11PixelShader * Basicpixelshader = NULL;


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