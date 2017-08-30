// CGS HW Project A "Line Land".
// Author: L.Norri CD CGS, FullSail University


#include <iostream>
#include <ctime>
#include "DDSTextureLoader.h"
#include "XTime.h"
#include "USERINPUT.h"
#include "Save.h"
#include "GAMEGPU.h"
#include "MENUGPU.h"
#include "GAME.h"
#include <windowsx.h>
using namespace std;

#include <d3d11.h>
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"User32.lib")

#include <DirectXMath.h>
using namespace DirectX;

#include <math.h>

int BACKBUFFER_WIDTH = 1920;
int BACKBUFFER_HEIGHT = 1080;

Save save;
USERINPUT input;

#pragma region software
class SOFTWARE
{
public:
	SOFTWARE(HINSTANCE hinst, WNDPROC proc);
	~SOFTWARE();
	bool Run();
	bool ShutDown();
private:
	HINSTANCE						application;
	WNDPROC							appWndProc;
	HWND							window;
	HRESULT							HR;

	MENUGPU * menugpu = nullptr;
	GAMEGPU * gamegpu = nullptr;
	GAME * game = nullptr;
	XTime Time;

};
SOFTWARE::SOFTWARE(HINSTANCE hinst, WNDPROC proc)
{
#pragma region wind
	application = hinst;
	appWndProc = proc;

	WNDCLASSEX  wndClass;
	ZeroMemory(&wndClass, sizeof(wndClass));
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.lpfnWndProc = appWndProc;
	wndClass.lpszClassName = L"DirectXApplication";
	wndClass.hInstance = application;
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
	//wndClass.hIcon			= LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_FSICON));
	RegisterClassEx(&wndClass);

	RECT window_size = { 0, 0, BACKBUFFER_WIDTH, BACKBUFFER_HEIGHT };
	AdjustWindowRect(&window_size, WS_OVERLAPPEDWINDOW, false);

	window = CreateWindow(L"DirectXApplication", L"Antonio Arbona Capstone", WS_OVERLAPPEDWINDOW & ~(WS_THICKFRAME | WS_MAXIMIZEBOX),
		CW_USEDEFAULT, CW_USEDEFAULT, window_size.right - window_size.left, window_size.bottom - window_size.top,
		NULL, NULL, application, this);

	ShowWindow(window, SW_SHOW);
#pragma endregion
	Time.Restart();
	menugpu = new MENUGPU(window);
	gamegpu = new GAMEGPU(window);
	game = new GAME(menugpu, gamegpu);

}
SOFTWARE::~SOFTWARE()
{
	//The gpu destructor is empty and needs the shutdown function to be called

}
bool SOFTWARE::Run()
{
	Time.Signal();
	game->Run(Time);
	return true;
}
bool SOFTWARE::ShutDown()
{
	//The gpu destructor is empty and needs the shutdown function to be called

	UnregisterClass(L"DirectXApplication", application);
	return true;
}
#pragma endregion

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wparam, LPARAM lparam);
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (GetAsyncKeyState(VK_ESCAPE))
		message = WM_DESTROY;
	switch (message)
	{
	case (WM_DESTROY):
	{
		PostQuitMessage(0);
		break;
	}
	case (WM_KEYDOWN):
	{
		if (wParam)
		{
			input.buttons[wParam] = true;
		}
		break;
	};
	case (WM_KEYUP):
	{
		if (wParam)
		{
			input.buttons[wParam] = false;
		}
		break;
	};
	case (WM_LBUTTONDOWN):
	{
		input.diffx = 0.0f;
		input.diffy = 0.0f;
		input.left_click = true;
		break;
	};
	case (WM_LBUTTONUP):
	{
		input.diffx = 0.0f;
		input.diffy = 0.0f;
		input.left_click = false;
		input.mouse_move = false;
		break;
	};	
	case (WM_RBUTTONDOWN):
	{
		input.right_click = true;
		break;
	};
	case (WM_RBUTTONUP):
	{
		input.right_click = false;
		break;
	};
	case (WM_MOUSEMOVE):
	{
		input.mouse_move = true;
		input.x = (float)GET_X_LPARAM(lParam);
		input.y = (float)GET_Y_LPARAM(lParam);
		input.diffx = input.x - input.prevX;
		input.diffy = input.y - input.prevY;
		input.prevX = input.x;
		input.prevY = input.y;
		break;
	};
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow);
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPTSTR, int)
{
	srand(unsigned int(time(0)));
	SOFTWARE software(hInstance, (WNDPROC)WndProc);
	MSG msg; ZeroMemory(&msg, sizeof(msg));
	tagTRACKMOUSEEVENT mouse_tracker; ZeroMemory(&mouse_tracker, sizeof(mouse_tracker));
	TrackMouseEvent(&mouse_tracker);
	while (msg.message != WM_QUIT && software.Run())
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg); //this calls the WndProc function
		}
	}
	software.ShutDown();
	return 0;
}
