#pragma once
#include "../FBX/FBX.h"
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#pragma comment(lib,"d3d11.lib")
using namespace DirectX;
using namespace std;
class RENDERABLECOMPONENT
{
public:
	RENDERABLECOMPONENT();
	~RENDERABLECOMPONENT();
	void initfbx(const void * pVSShaderByteCode, const void * pPSShaderByteCode, char * FileName);
	void inittridebug(const void * pVSShaderByteCode, size_t vssize, const void * pPSShaderByteCode, size_t pssize, SIMPLE_VERTEX * verts, unsigned int vertcount, unsigned int * indices, unsigned int indicescount);
	void initobj(const void * pVSShaderByteCode, size_t vssize, const void * pPSShaderByteCode, size_t pssize, char * FileName);
	void update(double dt);

	//private:
	//i should make getters and setters

	anim_clip * IdleAnimationData = nullptr;
	vert_pos_skinned * ptheverts = nullptr;
	unsigned int modelvertcount = 0;
	unsigned int * pvertindices = nullptr;
	unsigned int modelindexcount = 0;
	vector<joint> bind_pose{};
	unsigned int keyframecount = 0;
	unsigned int boneCount = 0;
	double animLoopTime = 0.0;
	XMFLOAT4X4 InverseBindPose[64];
	SIMPLE_VERTEX * realtimemodel = nullptr;
	
	//line to triangle variables
	size_t trianglecount = 0;
	TRIANGLE * triangles = nullptr;

	bool animationpaused = false;
	double curranimtime = 0.0;

	unsigned int keyframeanimindex = 0;
	double twokeyframetimes[2]{};
	ID3D11Buffer * constbuffer = nullptr;
	ID3D11Buffer * vertb = nullptr;
	ID3D11Buffer * indexb = nullptr;
	ID3D11VertexShader * vs = nullptr;
	ID3D11PixelShader * ps = nullptr;
	const void * vsbytecode = nullptr;
	size_t vssize = 0;
	const void * psbytecode = nullptr;
	size_t pssize = 0;
	ID3D11ShaderResourceView * texture = nullptr;
	ID3D11RasterizerState * rs = nullptr;
	D3D_PRIMITIVE_TOPOLOGY topology;
	ANIMATION_VRAM send_to_ram2{};
	D3D11_MAPPED_SUBRESOURCE mapResource{};

	UINT stride = sizeof(SIMPLE_VERTEX);
	UINT offset = 0;
};

