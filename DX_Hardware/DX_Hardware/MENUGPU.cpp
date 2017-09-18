#include "MENUGPU.h"

MENUGPU::MENUGPU(HWND &window)
{

#pragma region swap chain device context
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = window;
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;
	D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, 0, 0, 0, 0, D3D11_SDK_VERSION, &scd, &swapchain, &device, 0, &context);

#pragma endregion

#pragma region camera init

	XMMATRIX m = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 10.0f, 0.0f, 1.0f
	};
	//ten units in y  and 100 units in negative z 
	//save.LoadFromFile(camera);
	XMStoreFloat4x4(&camera, m);
	XMStoreFloat4x4(&send_to_ram.camView, XMMatrixTranspose(XMLoadFloat4x4(&camera)));

	float aspectRatio = (float)BACKBUFFER_WIDTH / (float)BACKBUFFER_HEIGHT;
	float fovAngleY = 70.0f * XM_PI / 180.0f;
	if (aspectRatio < 1.0f)
		fovAngleY *= 2.0f;
	fovAngleY = XMConvertToDegrees(fovAngleY);
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 10000.0f);
	XMStoreFloat4x4(&send_to_ram.camProj, XMMatrixTranspose(perspectiveMatrix));

	send_to_ram.spot_light_pos = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	send_to_ram.spot_light_dir = XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);


	ZeroMemory(&bufferdescription, sizeof(D3D11_BUFFER_DESC));
	bufferdescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferdescription.ByteWidth = sizeof(VRAM);
	bufferdescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferdescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferdescription.MiscFlags = NULL;
	bufferdescription.StructureByteStride = sizeof(VRAM);
	ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
	InitData.pSysMem = &send_to_ram;
	device->CreateBuffer(&bufferdescription, &InitData, &constBuffer);

#pragma endregion

#pragma region shaders and imput layout
	device->CreateVertexShader(MenuVertexShader, sizeof(MenuVertexShader), NULL, &Menuvertexshader);
	device->CreatePixelShader(MenuPixelShader, sizeof(MenuPixelShader), NULL, &Menupixelshader);

	D3D11_INPUT_ELEMENT_DESC vertlayout[] =
	{
		"POSITION", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0,
		"NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0,
		"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0,
		"UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0,
		"INDEX", 0,DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0,
		"WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0,
	};
	UINT numElements = ARRAYSIZE(vertlayout);
	device->CreateInputLayout(vertlayout, numElements, MenuVertexShader, sizeof(MenuVertexShader), &layout);
#pragma endregion

	menulistposition[EXIT] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 10.0f, 100.0f, 1.0f
	};
	InitalizeobjAsset(&exit, "exit_text.obj", L"white.dds", &menulistposition[EXIT]);
	//
	menulistposition[CREDITS] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 20.0f, 100.0f, 1.0f
	};
	InitalizeobjAsset(&credits, "credits_text.obj", L"white.dds", &menulistposition[CREDITS]);
	//
	menulistposition[NEWGAME] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 30.0f, 100.0f, 1.0f
	};
	InitalizeobjAsset(&newgame, "newgame_text.obj", L"white.dds", &menulistposition[NEWGAME]);
	//
	presenticonselected = EXIT;
	InitalizeobjAsset(&icon, "sphere.obj", L"black.dds", &menulistposition[presenticonselected]);

}

void MENUGPU::DrawToScreen()
{

	Clear();
	Set();

	RenderExact(&newgame, 1);
	RenderExact(&exit, 1);
	RenderExact(&credits, 1);
	RenderExact(&icon, 1);

	swapchain->Present(0, 0);
}

void MENUGPU::CameraUpdate(XTime &Time)
{

#pragma region debug scenechange
	if (!input.buttons[VK_RIGHT])
		input.buttonbuffer[VK_RIGHT] = false;
	if (input.buttonbuffer[VK_RIGHT] == false && input.buttons[VK_RIGHT])
	{
		loadgame = true;
		input.buttonbuffer[VK_RIGHT] = true;
	}
#pragma endregion
	
	if (!input.buttons[VK_DOWN])
		input.buttonbuffer[VK_DOWN] = false;
	if (input.buttonbuffer[VK_DOWN] == false && input.buttons[VK_DOWN])
	{
		if (presenticonselected == 0)
			presenticonselected = NEWGAME;
		else
			--presenticonselected;
		XMStoreFloat4x4(&icon.mesh.send_to_ram2.modelPos, XMMatrixTranspose(menulistposition[presenticonselected]));

		input.buttonbuffer[VK_DOWN] = true;
	}
	
	if (!input.buttons[VK_UP])
		input.buttonbuffer[VK_UP] = false;
	if (input.buttonbuffer[VK_UP] == false && input.buttons[VK_UP])
	{
		if (presenticonselected == NEWGAME)
			presenticonselected = EXIT;
		else
			++presenticonselected;
		XMStoreFloat4x4(&icon.mesh.send_to_ram2.modelPos, XMMatrixTranspose(menulistposition[presenticonselected]));
		input.buttonbuffer[VK_UP] = true;
	}



	XMMATRIX newcamera = XMLoadFloat4x4(&camera);

	if (input.buttons['W'])
		newcamera.r[3] = newcamera.r[3] + (newcamera.r[2] * +(float)Time.Delta() * 100.0f);
	if (input.buttons['S'])
		newcamera.r[3] = newcamera.r[3] + newcamera.r[2] * -(float)Time.Delta() * 100.0f;
	if (input.buttons['A'])
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * -(float)Time.Delta() * 100.0f;
	if (input.buttons['D'])
		newcamera.r[3] = newcamera.r[3] + newcamera.r[0] * +(float)Time.Delta() * 100.0f;
	if (input.mouse_move)
		if (input.left_click)
		{
			XMVECTOR pos = newcamera.r[3];
			XMFLOAT4 zeropos = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
			newcamera.r[3] = XMLoadFloat4(&zeropos);
			newcamera =
				XMMatrixRotationX(-input.diffy * (float)Time.Delta() * 10.0f)
				* newcamera
				* XMMatrixRotationY(-input.diffx * (float)Time.Delta() * 10.0f);
			newcamera.r[3] = pos;
		}
	input.mouse_move = false;
	XMStoreFloat4x4(&camera, newcamera);
	XMStoreFloat4x4(&send_to_ram.camView, XMMatrixTranspose(XMMatrixInverse(0, newcamera)));


	ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	context->Map(constBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
	memcpy(mapResource.pData, &send_to_ram, sizeof(VRAM));
	context->Unmap(constBuffer, 0);
}

void MENUGPU::RenderExact(OBJECT * object, unsigned int count)
{
	for (size_t i = 0; i < count; i++)
	{
		if (!object[i].alive)
			continue;
		ZeroMemory(&mapResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
		context->Map(object[i].mesh.constbuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapResource);
		memcpy(mapResource.pData, &object[i].mesh.send_to_ram2, sizeof(ANIMATION_VRAM));
		context->Unmap(object[i].mesh.constbuffer, 0);
		context->VSSetShader(object[i].mesh.vs, NULL, NULL);
		context->PSSetShader(object[i].mesh.ps, NULL, NULL);
		context->VSSetConstantBuffers(1, 1, &object[i].mesh.constbuffer);
		context->IASetPrimitiveTopology(object[i].mesh.topology);
		context->IASetVertexBuffers(0, 1, &object[i].mesh.vertb, &stride, &offset);
		context->IASetIndexBuffer(object[i].mesh.indexb, DXGI_FORMAT_R32_UINT, offset);
		context->RSSetState(object[i].mesh.rs);
		context->PSSetShaderResources(0, 1, &object[i].mesh.texture);
		context->DrawIndexed(object[i].mesh.modelindexcount, 0, 0);
	}
}

void MENUGPU::Set()
{
	context->OMSetRenderTargets(1, &rtv, depthStencilView);
	context->RSSetViewports(1, &viewport);
	context->VSSetConstantBuffers(0, 1, &constBuffer);
	context->PSSetConstantBuffers(0, 1, &constBuffer);
	context->IASetInputLayout(layout);
}

void MENUGPU::Clear()
{
	context->ClearRenderTargetView(rtv, color);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void MENUGPU::CreateRenderTargetView()
{
	ID3D11Texture2D * BackBuffer;
	ZeroMemory(&BackBuffer, sizeof(ID3D11Texture2D));
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);
	device->CreateRenderTargetView(BackBuffer, NULL, &rtv);
}

void MENUGPU::DefineViewPort()
{
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.Height = (float)BACKBUFFER_HEIGHT;
	viewport.Width = (float)BACKBUFFER_WIDTH;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
}

void MENUGPU::DefineAndCreateDepthStencil()
{
	CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, lround(BACKBUFFER_WIDTH), lround(BACKBUFFER_HEIGHT), 1, 1, D3D11_BIND_DEPTH_STENCIL);
	device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil);
	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	device->CreateDepthStencilView(depthStencil, &depthStencilViewDesc, &depthStencilView);
}

void MENUGPU::AllocateBuffer(OBJECT * object, wchar_t * texture)
{
	D3D11_BUFFER_DESC bufferdescription;
	D3D11_SUBRESOURCE_DATA InitData;
	D3D11_RASTERIZER_DESC wireFrameDesc;

	ZeroMemory(&bufferdescription, sizeof(D3D11_BUFFER_DESC));
	bufferdescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferdescription.ByteWidth = sizeof(ANIMATION_VRAM);
	bufferdescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferdescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferdescription.MiscFlags = NULL;
	bufferdescription.StructureByteStride = sizeof(ANIMATION_VRAM);
	ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
	InitData.pSysMem = &object->mesh.send_to_ram2;
	device->CreateBuffer(&bufferdescription, &InitData, &object->mesh.constbuffer);




	ZeroMemory(&bufferdescription, sizeof(D3D11_BUFFER_DESC));
	bufferdescription.Usage = D3D11_USAGE_IMMUTABLE;
	bufferdescription.ByteWidth = (UINT)(sizeof(SIMPLE_VERTEX) * object->mesh.modelvertcount);
	bufferdescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferdescription.CPUAccessFlags = NULL;
	bufferdescription.MiscFlags = NULL;
	bufferdescription.StructureByteStride = sizeof(SIMPLE_VERTEX);
	ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
	InitData.pSysMem = object->mesh.realtimemodel;
	device->CreateBuffer(&bufferdescription, &InitData, &object->mesh.vertb);
	ZeroMemory(&bufferdescription, sizeof(D3D11_BUFFER_DESC));
	bufferdescription.Usage = D3D11_USAGE_IMMUTABLE;
	bufferdescription.ByteWidth = (UINT)(sizeof(unsigned int) * object->mesh.modelindexcount);
	bufferdescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferdescription.CPUAccessFlags = NULL;
	bufferdescription.MiscFlags = NULL;
	bufferdescription.StructureByteStride = sizeof(unsigned int);
	ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
	InitData.pSysMem = object->mesh.pvertindices;
	device->CreateBuffer(&bufferdescription, &InitData, &object->mesh.indexb);
	CreateDDSTextureFromFile(device, texture, nullptr, &object->mesh.texture);
	device->CreateVertexShader(object->mesh.vsbytecode, object->mesh.vssize, NULL, &object->mesh.vs);
	device->CreatePixelShader(object->mesh.psbytecode, object->mesh.pssize, NULL, &object->mesh.ps);
	ZeroMemory(&wireFrameDesc, sizeof(D3D11_RASTERIZER_DESC));
	wireFrameDesc.FillMode = D3D11_FILL_SOLID;
	wireFrameDesc.CullMode = D3D11_CULL_BACK;
	wireFrameDesc.DepthClipEnable = true;
	device->CreateRasterizerState(&wireFrameDesc, &object->mesh.rs);

}

void MENUGPU::CreateConstantBuffer(void * data, unsigned int size, ID3D11Buffer * buffer)
{
	ZeroMemory(&bufferdescription, sizeof(D3D11_BUFFER_DESC));
	bufferdescription.Usage = D3D11_USAGE_DYNAMIC;
	bufferdescription.ByteWidth = size;
	bufferdescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferdescription.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bufferdescription.MiscFlags = NULL;
	bufferdescription.StructureByteStride = size;
	ZeroMemory(&InitData, sizeof(D3D11_SUBRESOURCE_DATA));
	InitData.pSysMem = data;
	device->CreateBuffer(&bufferdescription, &InitData, &buffer);

}

void MENUGPU::InitalizeobjAsset(OBJECT * object, char * mesh, wchar_t * texture, XMMATRIX * m)
{
	object->mesh.initobj(MenuVertexShader, sizeof(MenuVertexShader), MenuPixelShader, sizeof(MenuPixelShader), mesh);
	XMStoreFloat4x4(&object->mesh.send_to_ram2.modelPos, XMMatrixTranspose(*m));
	AllocateBuffer(object, texture);

}

void MENUGPU::InitalizeQuad(OBJECT * object, wchar_t * texture)
{
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
	object->mesh.inittridebug(MenuVertexShader, sizeof(MenuVertexShader), MenuPixelShader, sizeof(MenuPixelShader), quad.groundPlane, quad.vertcount, quad.groundPlaneindex, quad.indexcount);
	XMStoreFloat4x4(&object->mesh.send_to_ram2.modelPos, XMMatrixTranspose(XMMatrixIdentity()));
	AllocateBuffer(object, texture);
}

void MENUGPU::ShutDown()
{
	device->Release();
	context->Release();
	swapchain->Release();
	rtv->Release();
	depthStencil->Release();
	depthStencilView->Release();
	Menuvertexshader->Release();
	Menupixelshader->Release();
	layout->Release();
	constBuffer->Release();
}

MENUGPU::~MENUGPU()
{
	ShutDown();
}

