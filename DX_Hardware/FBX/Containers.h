#pragma once
#include <vector>
#include <DirectXMath.h>
using namespace DirectX;
using namespace std;

struct joint { XMFLOAT4X4 global_xform; int Parent_Index; };
struct keyframe { double Time; vector<XMFLOAT4X4> Joints; };
struct anim_clip { double Duration; vector<keyframe> Frames; };
struct vert_pos_skinned { XMFLOAT4 pos; XMFLOAT4 norm; vector<int> joints; vector<float> weights; XMFLOAT2 uv; };
struct SIMPLE_VERTEX { XMFLOAT4 xyzw; XMFLOAT4 normal; XMFLOAT4 color; XMFLOAT2 uv; XMFLOAT4 index; XMFLOAT4 weights; };
struct VRAM { XMFLOAT4X4 camView; XMFLOAT4X4 camProj; XMFLOAT4 spot_light_pos; XMFLOAT4 spot_light_dir; };
struct ANIMATION_VRAM { XMFLOAT4X4 modelPos; XMFLOAT4X4 RealTimePose[64]; };
struct TRIANGLE { XMFLOAT3 vertices[3]; XMFLOAT3 normal; };