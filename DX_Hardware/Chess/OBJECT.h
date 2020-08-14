#pragma once
#include "RENDERABLECOMPONENT.h"
//
#include "Containers.h"
#include <vector>
#include <d3d11.h>
#include <DirectXMath.h>
#pragma comment(lib,"d3d11.lib")
using namespace DirectX;
using namespace std;
//

class OBJECT
{
public:
	//RENDERABLECOMPONENT mesh;
	unsigned int positionindex[2];
	bool alive;
	bool ship;
	bool quad;
	bool dirty;// flag to update the mesh position to reflect the position index
	//TODO: add variables to allow the game to know what movement the ship can have

	enum SHIPMOVEHEADING { FORWARD = 1, ANGLE };
	enum SHIPMOVERANGE { ZERO, ONE, TWO, THREE, FOUR, TEN = 10 };
	enum HEADING { NONE, UP, UPRIGHT, RIGHT, DOWNRIGHT, DOWN, DOWNLEFT, LEFT, LEFTUP };
	unsigned int shipmoveheading;//ship moves n, e, s, w, or ne, se, sw, nw
	unsigned int shipmoverange;//most tiles ship can traverse in one turn
	unsigned int shipmovecount;//spaces moved in relation to ship range(used to move )
	unsigned int shipselectedheading;//the heading that player selected to move ship in 


	OBJECT();
	~OBJECT();


	void inittridebug(const void* pVSShaderByteCode, size_t vssize, const void* pPSShaderByteCode, size_t pssize, SIMPLE_VERTEX* verts, unsigned int vertcount, unsigned int* indices, unsigned int indicescount);
	void initobj(const void* pVSShaderByteCode, size_t vssize, const void* pPSShaderByteCode, size_t pssize, char* FileName);

	//private:
	//i should make getters and setters

	anim_clip* IdleAnimationData = nullptr;
	vert_pos_skinned* ptheverts = nullptr;
	unsigned int modelvertcount = 0;
	unsigned int* pvertindices = nullptr;
	unsigned int modelindexcount = 0;
	vector<joint> bind_pose{};
	unsigned int keyframecount = 0;
	unsigned int boneCount = 0;
	double animLoopTime = 0.0;
	XMFLOAT4X4 InverseBindPose[64];
	SIMPLE_VERTEX* realtimemodel = nullptr;

	bool animationpaused = false;
	double curranimtime = 0.0;

	unsigned int keyframeanimindex = 0;
	double twokeyframetimes[2]{};
	ID3D11Buffer* constbuffer = nullptr;
	ID3D11Buffer* vertb = nullptr;
	ID3D11Buffer* indexb = nullptr;
	ID3D11VertexShader* vs = nullptr;
	ID3D11PixelShader* ps = nullptr;
	const void* vsbytecode = nullptr;
	size_t vssize = 0;
	const void* psbytecode = nullptr;
	size_t pssize = 0;
	ID3D11ShaderResourceView* texture = nullptr;
	ID3D11RasterizerState* rs = nullptr;
	D3D_PRIMITIVE_TOPOLOGY topology;
	ANIMATION_VRAM send_to_ram2{};
	D3D11_MAPPED_SUBRESOURCE mapResource{};

	UINT stride = sizeof(SIMPLE_VERTEX);
	UINT offset = 0;

};

