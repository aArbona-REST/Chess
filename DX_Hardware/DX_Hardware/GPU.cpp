#include "GPU.h"


GPU::GPU(HWND &window)
{
	this->window = &window;

#pragma region swap chain device context
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferCount = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = *(this->window);
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
	XMMATRIX perspectiveMatrix = XMMatrixPerspectiveFovLH(fovAngleY, aspectRatio, 0.01f, 100000.0f);
	XMStoreFloat4x4(&send_to_ram.camProj, XMMatrixTranspose(perspectiveMatrix));

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
	device->CreateVertexShader(BasicVertexShader, sizeof(BasicVertexShader), NULL, &Basicvertexshader);
	device->CreatePixelShader(BasicPixelShader, sizeof(BasicPixelShader), NULL, &Basicpixelshader);

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
	device->CreateInputLayout(vertlayout, numElements, BasicVertexShader, sizeof(BasicVertexShader), &layout);
#pragma endregion

#pragma region generate map


	float xcurrentslice = 0.5f, ycurrentslice = 0.0f, zcurrentslice = 0.5f, x = 0.0f, y = 0.0f, z = 0.0f;

	float slice = 360.0f / 14.0f, initialradius = 20.0f, radiusincrease = 10.0f, liveradius = initialradius;

	XMFLOAT4 position = XMFLOAT4();
	for (size_t r = 0; r < row; r++)
	{
		x = sin((xcurrentslice * XM_PI) / 180);
		y = tan((ycurrentslice * XM_PI) / 180);
		z = cos((zcurrentslice * XM_PI) / 180);

		for (size_t d = 0; d < depth; d++)
		{
			position = XMFLOAT4((x * liveradius), (y * liveradius), (z * liveradius), 1.0f);
			map[r][d].positionindex[0] = (unsigned int)r;
			map[r][d].positionindex[1] = (unsigned int)d;
			map[r][d].positionstatus = 0;
			map[r][d].occupieindex = 0;
			map[r][d].position = position;
			if (d == 0)
				map[r][d].front = nullptr;
			else
				map[r][d].front = &(map[r][d - 1]);
			if (d == depth - 1)
				map[r][d].back = nullptr;
			else
				map[r][d].back = &(map[r][d + 1]);
			if (r == 0)
				map[r][d].left = &(map[row - 1][d]);
			else
				map[r][d].left = &(map[r - 1][d]);
			if (r == row - 1)
				map[r][d].right = &(map[row - row][d]);
			else
				map[r][d].right = &(map[r + 1][d]);
			liveradius += radiusincrease;
		}
		xcurrentslice += slice;
		zcurrentslice += slice;
		liveradius = initialradius;
	}
#pragma endregion

#pragma region load assets
	for (size_t r = 0; r < row; r++)
	{
		for (size_t d = 0; d < depth; d++)
		{
			quadsmap[r][d].mesh.send_to_ram2.modelPos._14 = map[r][d].position.x;
			quadsmap[r][d].mesh.send_to_ram2.modelPos._24 = map[r][d].position.y;
			quadsmap[r][d].mesh.send_to_ram2.modelPos._34 = map[r][d].position.z;
			quadsmap[r][d].mesh.send_to_ram2.modelPos._44 = map[r][d].position.w;
			quadsmap[r][d].positionindex[0] = (unsigned int)r;
			quadsmap[r][d].positionindex[1] = (unsigned int)d;
			//replace this testcube.dds with one that is easy on the eyes. (Eg. all white)
			InitalizeQuad(&quadsmap[r][d], L"TestCube.dds");
		}
	}
	InitalizeQuad(&billboard, L"TestCube.dds");

	InitalizePlayerAssets(1, teamone, "talon.obj", L"talon.dds");
	InitalizePlayerAssets(2, teamtwo, "talon.obj", L"gladiator.dds");
	InitalizeobjAsset(&selectedobjecticon, "sphere.obj", L"talon.dds");
#pragma endregion

}

void GPU::DrawToScreen()
{

	Clear();
	Set();
	Render(teamone, piececount);
	Render(teamtwo, piececount);
	Render(&selectedobjecticon, 1);
	for (size_t r = 0; r < row; r++)
		for (size_t d = 0; d < depth; d++)
			Render(&quadsmap[r][d], 1);

#pragma region billboard example


	XMVECTOR billboardpos = XMLoadFloat4x4(&billboard.mesh.send_to_ram2.modelPos).r[3];

	XMFLOAT4 up = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMVECTOR billboardup = XMLoadFloat4(&up);

	XMVECTOR camerapos = XMLoadFloat4x4(&camera).r[3];

	XMMATRIX m = XMMatrixLookAtLH(billboardpos, billboardup, camerapos);

	XMStoreFloat4x4(&billboard.mesh.send_to_ram2.modelPos, m);



	RenderExact(&billboard, 1);
#pragma endregion

	swapchain->Present(0, 0);
}

void GPU::PlayerInput(OBJECT * objects, unsigned int playerteam, unsigned int enemyteam)
{

	if (!input.buttons[VK_END])
		input.buttonbuffer[VK_END] = false;
	if (input.buttonbuffer[VK_END] == false && input.buttons[VK_END])
	{
		save.SaveToFile(camera);
		input.buttonbuffer[VK_END] = true;
	}
	if (!input.buttons[VK_RETURN])
		input.buttonbuffer[VK_RETURN] = false;
	if (input.buttonbuffer[VK_RETURN] == false && input.buttons[VK_RETURN])
	{
		unsigned int row = selectedobjecticon.positionindex[0];
		unsigned int depth = selectedobjecticon.positionindex[1];
		if (map[row][depth].positionstatus == playerteam)
		{
			pieceselected = !pieceselected;
			if (pieceselected)
				selectedobject = map[row][depth].occupieindex;
			else
				selectedobject = -1;
		}
		input.buttonbuffer[VK_RETURN] = true;
	}

	//TODO: add a switch
	//switch used for reading selected piece and
	//moving the piece where it is allowed to move according to the game piece rules

#pragma region left
	if (!input.buttons[VK_LEFT])
		input.buttonbuffer[VK_LEFT] = false;
	if (input.buttonbuffer[VK_LEFT] == false && input.buttons[VK_LEFT])
	{
		unsigned int row = selectedobjecticon.positionindex[0];
		unsigned int depth = selectedobjecticon.positionindex[1];
		if (!pieceselected)
		{
			selectedobjecticon.positionindex[0] = map[row][depth].left->positionindex[0];
			selectedobjecticon.positionindex[1] = map[row][depth].left->positionindex[1];
		}
		else
		{
			switch (map[row][depth].left->positionstatus)
			{
			case 0:
			{
				selectedobjecticon.positionindex[0] = map[row][depth].left->positionindex[0];
				selectedobjecticon.positionindex[1] = map[row][depth].left->positionindex[1];
				map[row][depth].positionstatus = 0;
				map[row][depth].left->positionstatus = playerteam;
				unsigned int objectindex = map[row][depth].occupieindex;
				map[row][depth].occupieindex = -1;
				map[row][depth].left->occupieindex = objectindex;
				if (playerteam == 1)
				{
					teamone[selectedobject].positionindex[0] = map[row][depth].left->positionindex[0];
					teamone[selectedobject].positionindex[1] = map[row][depth].left->positionindex[1];
				}
				else if (playerteam == 2)
				{
					teamtwo[selectedobject].positionindex[0] = map[row][depth].left->positionindex[0];
					teamtwo[selectedobject].positionindex[1] = map[row][depth].left->positionindex[1];
				}
				pieceselected = !pieceselected;
				selectedobject = -1;
				turnended = true;
				break;
			}
			case 1:
			{

				switch (playerteam)
				{
				case 2:
				{
					teamone[map[row][depth].left->occupieindex].alive = false;

					selectedobjecticon.positionindex[0] = map[row][depth].left->positionindex[0];
					selectedobjecticon.positionindex[1] = map[row][depth].left->positionindex[1];
					map[row][depth].positionstatus = 0;
					map[row][depth].left->positionstatus = playerteam;
					unsigned int objectindex = map[row][depth].occupieindex;
					map[row][depth].occupieindex = -1;
					map[row][depth].left->occupieindex = objectindex;


					teamtwo[selectedobject].positionindex[0] = map[row][depth].left->positionindex[0];
					teamtwo[selectedobject].positionindex[1] = map[row][depth].left->positionindex[1];
					pieceselected = !pieceselected;
					selectedobject = -1;
					turnended = true;
					teamonecount--;
					break;
				}
				}
				break;
			}
			case 2:
			{
				switch (playerteam)
				{
				case 1:
				{
					teamtwo[map[row][depth].left->occupieindex].alive = false;

					selectedobjecticon.positionindex[0] = map[row][depth].left->positionindex[0];
					selectedobjecticon.positionindex[1] = map[row][depth].left->positionindex[1];
					map[row][depth].positionstatus = 0;
					map[row][depth].left->positionstatus = playerteam;
					unsigned int objectindex = map[row][depth].occupieindex;
					map[row][depth].occupieindex = -1;
					map[row][depth].left->occupieindex = objectindex;


					teamone[selectedobject].positionindex[0] = map[row][depth].left->positionindex[0];
					teamone[selectedobject].positionindex[1] = map[row][depth].left->positionindex[1];
					pieceselected = !pieceselected;
					selectedobject = -1;
					turnended = true;
					teamtwocount--;
					break;
				}
				}
				break;
			}
			}
		}
		input.buttonbuffer[VK_LEFT] = true;
	}
#pragma endregion

#pragma region right

	if (!input.buttons[VK_RIGHT])
		input.buttonbuffer[VK_RIGHT] = false;
	if (input.buttonbuffer[VK_RIGHT] == false && input.buttons[VK_RIGHT])
	{
		unsigned int row = selectedobjecticon.positionindex[0];
		unsigned int depth = selectedobjecticon.positionindex[1];
		if (!pieceselected)
		{
			selectedobjecticon.positionindex[0] = map[row][depth].right->positionindex[0];
			selectedobjecticon.positionindex[1] = map[row][depth].right->positionindex[1];
		}
		else
		{
			switch (map[row][depth].right->positionstatus)
			{
			case 0:
			{
				selectedobjecticon.positionindex[0] = map[row][depth].right->positionindex[0];
				selectedobjecticon.positionindex[1] = map[row][depth].right->positionindex[1];
				map[row][depth].positionstatus = 0;
				map[row][depth].right->positionstatus = playerteam;
				unsigned int objectindex = map[row][depth].occupieindex;
				map[row][depth].occupieindex = -1;
				map[row][depth].right->occupieindex = objectindex;
				if (playerteam == 1)
				{
					teamone[selectedobject].positionindex[0] = map[row][depth].right->positionindex[0];
					teamone[selectedobject].positionindex[1] = map[row][depth].right->positionindex[1];
				}
				else if (playerteam == 2)
				{
					teamtwo[selectedobject].positionindex[0] = map[row][depth].right->positionindex[0];
					teamtwo[selectedobject].positionindex[1] = map[row][depth].right->positionindex[1];
				}
				pieceselected = !pieceselected;
				selectedobject = -1;
				turnended = true;
				break;
			}
			case 1:
			{

				switch (playerteam)
				{
				case 2:
				{
					teamone[map[row][depth].right->occupieindex].alive = false;

					selectedobjecticon.positionindex[0] = map[row][depth].right->positionindex[0];
					selectedobjecticon.positionindex[1] = map[row][depth].right->positionindex[1];
					map[row][depth].positionstatus = 0;
					map[row][depth].right->positionstatus = playerteam;
					unsigned int objectindex = map[row][depth].occupieindex;
					map[row][depth].occupieindex = -1;
					map[row][depth].right->occupieindex = objectindex;


					teamtwo[selectedobject].positionindex[0] = map[row][depth].right->positionindex[0];
					teamtwo[selectedobject].positionindex[1] = map[row][depth].right->positionindex[1];
					pieceselected = !pieceselected;
					selectedobject = -1;
					turnended = true;
					teamonecount--;
					break;
				}
				}
				break;
			}
			case 2:
			{
				switch (playerteam)
				{
				case 1:
				{
					teamtwo[map[row][depth].right->occupieindex].alive = false;

					selectedobjecticon.positionindex[0] = map[row][depth].right->positionindex[0];
					selectedobjecticon.positionindex[1] = map[row][depth].right->positionindex[1];
					map[row][depth].positionstatus = 0;
					map[row][depth].right->positionstatus = playerteam;
					unsigned int objectindex = map[row][depth].occupieindex;
					map[row][depth].occupieindex = -1;
					map[row][depth].right->occupieindex = objectindex;


					teamone[selectedobject].positionindex[0] = map[row][depth].right->positionindex[0];
					teamone[selectedobject].positionindex[1] = map[row][depth].right->positionindex[1];
					pieceselected = !pieceselected;
					selectedobject = -1;
					turnended = true;
					teamtwocount--;
					break;
				}
				}
				break;
			}
			}
		}
		input.buttonbuffer[VK_RIGHT] = true;
	}
#pragma endregion

#pragma region down
	if (!input.buttons[VK_DOWN])
		input.buttonbuffer[VK_DOWN] = false;
	if (input.buttonbuffer[VK_DOWN] == false && input.buttons[VK_DOWN])
	{
		unsigned int row = selectedobjecticon.positionindex[0];
		unsigned int depth = selectedobjecticon.positionindex[1];
		if (!pieceselected && map[row][depth].front != nullptr)
		{
			selectedobjecticon.positionindex[0] = map[row][depth].front->positionindex[0];
			selectedobjecticon.positionindex[1] = map[row][depth].front->positionindex[1];
		}
		else
		{
			if (map[row][depth].front != nullptr)
			{

				switch (map[row][depth].front->positionstatus)
				{
				case 0:
				{
					selectedobjecticon.positionindex[0] = map[row][depth].front->positionindex[0];
					selectedobjecticon.positionindex[1] = map[row][depth].front->positionindex[1];
					map[row][depth].positionstatus = 0;
					map[row][depth].front->positionstatus = playerteam;
					unsigned int objectindex = map[row][depth].occupieindex;
					map[row][depth].occupieindex = -1;
					map[row][depth].front->occupieindex = objectindex;
					if (playerteam == 1)
					{
						teamone[selectedobject].positionindex[0] = map[row][depth].front->positionindex[0];
						teamone[selectedobject].positionindex[1] = map[row][depth].front->positionindex[1];
					}
					else if (playerteam == 2)
					{
						teamtwo[selectedobject].positionindex[0] = map[row][depth].front->positionindex[0];
						teamtwo[selectedobject].positionindex[1] = map[row][depth].front->positionindex[1];
					}
					//TODO: the pieceselected, selectedobject, and turnended
					//some how make them toggle only when the player has moved
					//the selected piece one space at least, or to the pieces max range
					pieceselected = !pieceselected;
					selectedobject = -1;
					turnended = true;
					//
					//TODO: sound code(low priority)
					break;
				}
				case 1:
				{
					switch (playerteam)
					{
					case 2:
					{
						teamone[map[row][depth].front->occupieindex].alive = false;

						selectedobjecticon.positionindex[0] = map[row][depth].front->positionindex[0];
						selectedobjecticon.positionindex[1] = map[row][depth].front->positionindex[1];
						map[row][depth].positionstatus = 0;
						map[row][depth].front->positionstatus = playerteam;
						unsigned int objectindex = map[row][depth].occupieindex;
						map[row][depth].occupieindex = -1;
						map[row][depth].front->occupieindex = objectindex;


						teamtwo[selectedobject].positionindex[0] = map[row][depth].front->positionindex[0];
						teamtwo[selectedobject].positionindex[1] = map[row][depth].front->positionindex[1];
						pieceselected = !pieceselected;
						selectedobject = -1;
						turnended = true;
						teamonecount--;
						break;
					}
					}
					break;
				}
				case 2:
				{

					switch (playerteam)
					{
					case 1:
					{
						teamtwo[map[row][depth].front->occupieindex].alive = false;

						selectedobjecticon.positionindex[0] = map[row][depth].front->positionindex[0];
						selectedobjecticon.positionindex[1] = map[row][depth].front->positionindex[1];
						map[row][depth].positionstatus = 0;
						map[row][depth].front->positionstatus = playerteam;
						unsigned int objectindex = map[row][depth].occupieindex;
						map[row][depth].occupieindex = -1;
						map[row][depth].front->occupieindex = objectindex;


						teamone[selectedobject].positionindex[0] = map[row][depth].front->positionindex[0];
						teamone[selectedobject].positionindex[1] = map[row][depth].front->positionindex[1];
						pieceselected = !pieceselected;
						selectedobject = -1;
						turnended = true;
						teamtwocount--;
						break;
					}
					}
					break;
				}
				}
			}
		}
		input.buttonbuffer[VK_DOWN] = true;
	}
#pragma endregion

#pragma region up
	if (!input.buttons[VK_UP])
		input.buttonbuffer[VK_UP] = false;
	if (input.buttonbuffer[VK_UP] == false && input.buttons[VK_UP])
	{
		unsigned int row = selectedobjecticon.positionindex[0];
		unsigned int depth = selectedobjecticon.positionindex[1];
		if (!pieceselected && map[row][depth].back != nullptr)
		{
			selectedobjecticon.positionindex[0] = map[row][depth].back->positionindex[0];
			selectedobjecticon.positionindex[1] = map[row][depth].back->positionindex[1];
		}
		else
		{
			if (map[row][depth].back != nullptr)
			{
				switch (map[row][depth].back->positionstatus)
				{
				case 0:
				{
					selectedobjecticon.positionindex[0] = map[row][depth].back->positionindex[0];
					selectedobjecticon.positionindex[1] = map[row][depth].back->positionindex[1];
					map[row][depth].positionstatus = 0;
					map[row][depth].back->positionstatus = playerteam;
					unsigned int objectindex = map[row][depth].occupieindex;
					map[row][depth].occupieindex = -1;
					map[row][depth].back->occupieindex = objectindex;
					if (playerteam == 1)
					{
						teamone[selectedobject].positionindex[0] = map[row][depth].back->positionindex[0];
						teamone[selectedobject].positionindex[1] = map[row][depth].back->positionindex[1];
					}
					else if (playerteam == 2)
					{
						teamtwo[selectedobject].positionindex[0] = map[row][depth].back->positionindex[0];
						teamtwo[selectedobject].positionindex[1] = map[row][depth].back->positionindex[1];
					}
					pieceselected = !pieceselected;
					selectedobject = -1;
					turnended = true;
					break;
				}
				case 1:
				{
					switch (playerteam)
					{
					case 2:
					{
						teamone[map[row][depth].back->occupieindex].alive = false;

						selectedobjecticon.positionindex[0] = map[row][depth].back->positionindex[0];
						selectedobjecticon.positionindex[1] = map[row][depth].back->positionindex[1];
						map[row][depth].positionstatus = 0;
						map[row][depth].back->positionstatus = playerteam;
						unsigned int objectindex = map[row][depth].occupieindex;
						map[row][depth].occupieindex = -1;
						map[row][depth].back->occupieindex = objectindex;


						teamtwo[selectedobject].positionindex[0] = map[row][depth].back->positionindex[0];
						teamtwo[selectedobject].positionindex[1] = map[row][depth].back->positionindex[1];
						pieceselected = !pieceselected;
						selectedobject = -1;
						turnended = true;
						teamonecount--;
						break;
					}
					}
					break;
				}
				case 2:
				{
					switch (playerteam)
					{
					case 1:
					{
						teamtwo[map[row][depth].back->occupieindex].alive = false;

						selectedobjecticon.positionindex[0] = map[row][depth].back->positionindex[0];
						selectedobjecticon.positionindex[1] = map[row][depth].back->positionindex[1];
						map[row][depth].positionstatus = 0;
						map[row][depth].back->positionstatus = playerteam;
						unsigned int objectindex = map[row][depth].occupieindex;
						map[row][depth].occupieindex = -1;
						map[row][depth].back->occupieindex = objectindex;


						teamone[selectedobject].positionindex[0] = map[row][depth].back->positionindex[0];
						teamone[selectedobject].positionindex[1] = map[row][depth].back->positionindex[1];
						pieceselected = !pieceselected;
						selectedobject = -1;
						turnended = true;
						teamtwocount--;
						break;
					}
					}
					break;
				}
				}

			}
		}
		input.buttonbuffer[VK_UP] = true;
	}
#pragma endregion

}

void GPU::CameraUpdate(XTime &Time)
{

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

void GPU::RenderExact(OBJECT * object, unsigned int count)
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

void GPU::Render(OBJECT * object, unsigned int count)
{
	for (size_t i = 0; i < count; i++)
	{
		if (!object[i].alive)
			continue;
		object[i].mesh.send_to_ram2.modelPos._14 = map[object[i].positionindex[0]][object[i].positionindex[1]].position.x;
		object[i].mesh.send_to_ram2.modelPos._24 = map[object[i].positionindex[0]][object[i].positionindex[1]].position.y;
		object[i].mesh.send_to_ram2.modelPos._34 = map[object[i].positionindex[0]][object[i].positionindex[1]].position.z;
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

void GPU::Set()
{
	context->OMSetRenderTargets(1, &rtv, depthStencilView);
	context->RSSetViewports(1, &viewport);
	context->VSSetConstantBuffers(0, 1, &constBuffer);
	context->PSSetConstantBuffers(0, 1, &constBuffer);
	context->IASetInputLayout(layout);
}

void GPU::Clear()
{
	context->ClearRenderTargetView(rtv, color);
	context->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
}

void GPU::CreateRenderTargetView()
{
	ID3D11Texture2D * BackBuffer;
	ZeroMemory(&BackBuffer, sizeof(ID3D11Texture2D));
	swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&BackBuffer);
	device->CreateRenderTargetView(BackBuffer, NULL, &rtv);
}

void GPU::DefineViewPort()
{
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1;
	viewport.Height = (float)BACKBUFFER_HEIGHT;
	viewport.Width = (float)BACKBUFFER_WIDTH;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
}

void GPU::DefineAndCreateDepthStencil()
{
	CD3D11_TEXTURE2D_DESC depthStencilDesc(DXGI_FORMAT_D24_UNORM_S8_UINT, lround(BACKBUFFER_WIDTH), lround(BACKBUFFER_HEIGHT), 1, 1, D3D11_BIND_DEPTH_STENCIL);
	device->CreateTexture2D(&depthStencilDesc, nullptr, &depthStencil);
	CD3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc(D3D11_DSV_DIMENSION_TEXTURE2D);
	device->CreateDepthStencilView(depthStencil, &depthStencilViewDesc, &depthStencilView);
}

void GPU::AllocateBuffer(OBJECT * object, wchar_t * texture)
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

void GPU::CreateConstantBuffer(void * data, unsigned int size, ID3D11Buffer * buffer)
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

void GPU::InitalizePlayerAssets(unsigned int team, OBJECT * object, char * mesh, wchar_t * texture)
{
	unsigned int init_row = 0;
	unsigned int init_depth = 0;
	switch (team)
	{
	case 1:
		init_row = 0;
		init_depth = 1;
		break;
	case 2:
		init_row = 7;
		init_depth = 1;
		break;
	}
	for (size_t i = 0; i < teamcapacity; i++)
	{
		map[init_row][init_depth].positionstatus = team;
		map[init_row][init_depth].occupieindex = (unsigned int)i;

		object[i].positionindex[0] = init_row;
		object[i].positionindex[1] = init_depth;
		init_depth += 1;
		if (init_depth == 7)
		{
			init_row += 1;
			init_depth = 1;
		}
		XMStoreFloat4x4(&object[i].mesh.send_to_ram2.modelPos, XMMatrixTranspose(XMMatrixIdentity()));

		switch (i)
		{
		case 0:
		{
			LoadDestroyer(team, &object[i]);
			break;
		}
		case 1:
		{
			LoadFrigate(team, &object[i]);
			break;
		}
		case 2:
		{
			LoadFighter(team, &object[i]);
			break;
		}
		case 3:
		{
			LoadFighter(team, &object[i]);
			break;
		}
		case 4:
		{
			LoadFrigate(team, &object[i]);
			break;
		}
		case 5:
		{
			LoadDestroyer(team, &object[i]);
			break;
		}
		case 6:
		{
			LoadCruiser(team, &object[i]);
			break;
		}
		case 7:
		{
			LoadBattleship(team, &object[i]);
			break;
		}
		case 8:
		{
			LoadTitan(team, &object[i]);
			break;
		}
		case 9:
		{
			LoadTitan(team, &object[i]);
			break;
		}
		case 10:
		{
			LoadBattleship(team, &object[i]);
			break;
		}
		case 11:
		{
			LoadCruiser(team, &object[i]);
			break;
		}
		case 12:
		{
			LoadDestroyer(team, &object[i]);
			break;
		}
		case 13:
		{
			LoadFrigate(team, &object[i]);
			break;
		}
		case 14:
		{
			LoadFighter(team, &object[i]);
			break;
		}
		case 15:
		{
			LoadFighter(team, &object[i]);
			break;
		}
		case 16:
		{
			LoadFrigate(team, &object[i]);
			break;
		}
		case 17:
		{
			LoadDestroyer(team, &object[i]);
			break;
		}
		}

		//object[i].mesh.initobj(BasicVertexShader, sizeof(BasicVertexShader), BasicPixelShader, sizeof(BasicPixelShader), mesh);
		//AllocateBuffer(&object[i], texture);

	}
}

void GPU::LoadTitan(unsigned int team, OBJECT * object)
{
	object->mesh.initobj(BasicVertexShader, sizeof(BasicVertexShader), BasicPixelShader, sizeof(BasicPixelShader), "talon.obj");
	AllocateBuffer(object, L"talon.dds");
	switch (team)
	{
	case 1:
	{
		break;
	}
	case 2:
	{

		break;
	}
	}
}

void GPU::LoadBattleship(unsigned int team, OBJECT * object)
{
	object->mesh.initobj(BasicVertexShader, sizeof(BasicVertexShader), BasicPixelShader, sizeof(BasicPixelShader), "talon.obj");
	AllocateBuffer(object, L"talon.dds");

}

void GPU::LoadCruiser(unsigned int team, OBJECT * object)
{
	object->mesh.initobj(BasicVertexShader, sizeof(BasicVertexShader), BasicPixelShader, sizeof(BasicPixelShader), "talon.obj");
	AllocateBuffer(object, L"talon.dds");

}

void GPU::LoadFrigate(unsigned int team, OBJECT * object)
{
	object->mesh.initobj(BasicVertexShader, sizeof(BasicVertexShader), BasicPixelShader, sizeof(BasicPixelShader), "talon.obj");
	AllocateBuffer(object, L"talon.dds");

}

void GPU::LoadDestroyer(unsigned int team, OBJECT * object)
{
	object->mesh.initobj(BasicVertexShader, sizeof(BasicVertexShader), BasicPixelShader, sizeof(BasicPixelShader), "talon.obj");
	AllocateBuffer(object, L"talon.dds");

}

void GPU::LoadFighter(unsigned int team, OBJECT * object)
{
	object->mesh.initobj(BasicVertexShader, sizeof(BasicVertexShader), BasicPixelShader, sizeof(BasicPixelShader), "talon.obj");
	AllocateBuffer(object, L"talon.dds");

}

void GPU::InitalizeobjAsset(OBJECT * object, char * mesh, wchar_t * texture)
{
	object->mesh.initobj(BasicVertexShader, sizeof(BasicVertexShader), BasicPixelShader, sizeof(BasicPixelShader), mesh);
	XMStoreFloat4x4(&object->mesh.send_to_ram2.modelPos, XMMatrixTranspose(XMMatrixIdentity()));
	AllocateBuffer(object, texture);

}

void GPU::InitalizeQuad(OBJECT * object, wchar_t * texture)
{

	object->mesh.inittridebug(BasicVertexShader, sizeof(BasicVertexShader), BasicPixelShader, sizeof(BasicPixelShader), quad.groundPlane, quad.vertcount, quad.groundPlaneindex, quad.indexcount);
	XMStoreFloat4x4(&object->mesh.send_to_ram2.modelPos, XMMatrixTranspose(XMMatrixIdentity()));
	AllocateBuffer(object, texture);
}

bool GPU::ShutDown()
{
	save.SaveToFile(camera);
	device->Release();
	swapchain->Release();
	context->Release();
	rtv->Release();
	depthStencil->Release();
	depthStencilView->Release();
	layout->Release();
	constBuffer->Release();
	return true;
}

GPU::~GPU()
{
}

