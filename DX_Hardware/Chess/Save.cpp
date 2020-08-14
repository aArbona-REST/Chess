#include "Save.h"

Save::Save()
{
}
void Save::LoadFromFile(XMFLOAT4X4 &data)
{
	f = nullptr;
	error = fopen_s(&f, file, "rb");
	if (error)
		return;
	fread(&data, sizeof(XMFLOAT4X4), 1, f);
	fclose(f);
	f = nullptr;
}
void Save::SaveToFile(XMFLOAT4X4 data)
{
	error = fopen_s(&f, file, "wb");
	if (f)
		fwrite(&data, sizeof(XMFLOAT4X4), 1, f);
	fclose(f);
	f = nullptr;
}
Save::~Save()
{
}