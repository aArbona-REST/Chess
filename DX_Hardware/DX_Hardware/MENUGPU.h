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
	MENUGPU(HWND &window);
	~MENUGPU();
	void DrawToScreen();
	void ShutDown();
	void MENUGPU::InitalizeobjAsset(OBJECT * object, char * mesh, wchar_t * texture, XMMATRIX * m);
	void MENUGPU::InitalizeQuad(OBJECT * object, wchar_t * texture);
	void MENUGPU::AllocateBuffer(OBJECT * object, wchar_t * texture);
	void MENUGPU::CreateConstantBuffer(void * data, unsigned int size, ID3D11Buffer * buffer);//bugged function and does not work. 
	void MENUGPU::CameraUpdate(XTime &Time);
	void MENUGPU::RenderExact(OBJECT * object, unsigned int count);
	void MENUGPU::CreateRenderTargetView();
	void MENUGPU::DefineViewPort();
	void MENUGPU::DefineAndCreateDepthStencil();

	bool loadgame = false, shutdown = false;
private:

	void MENUGPU::Set();
	void MENUGPU::Clear();

	float color[4]{ 0.2f, 0.2f, 0.5f, 0.0f };
	unsigned int presenticonselected;
	XMMATRIX menulistposition[3];
	enum MENULIST { EXIT, CREDITS, NEWGAME };
	OBJECT newgame, exit, credits, icon;
	D3D11_BUFFER_DESC bufferdescription;
	D3D11_SUBRESOURCE_DATA InitData;
	ID3D11Device *device = NULL;
	ID3D11DeviceContext * context = NULL;
	IDXGISwapChain *swapchain = NULL;
	ID3D11RenderTargetView * rtv = NULL;
	D3D11_VIEWPORT viewport;
	ID3D11Texture2D * depthStencil = nullptr;
	ID3D11DepthStencilView * depthStencilView = nullptr;
	ID3D11VertexShader * Menuvertexshader = NULL;
	ID3D11PixelShader * Menupixelshader = NULL;
	ID3D11InputLayout * layout = NULL;
	ID3D11Buffer * constBuffer = NULL;
	VRAM send_to_ram;
	XMFLOAT4X4 camera;
	UINT stride = sizeof(SIMPLE_VERTEX);
	UINT offset = 0;
	D3D11_MAPPED_SUBRESOURCE mapResource;
};
