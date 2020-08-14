#pragma once
#include <iostream>
#include <DirectXMath.h>
using namespace DirectX;

class Save
{
public:
	Save();
	void LoadFromFile(XMFLOAT4X4 &data);
	void SaveToFile(XMFLOAT4X4 data);
	~Save();
private:
	char file[9]{ "save.bin" };
	FILE * f;
	int error = 0;
};