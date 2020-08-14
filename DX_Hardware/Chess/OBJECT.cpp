#include "OBJECT.h"



OBJECT::OBJECT()
{
	positionindex[0] = 0;
	positionindex[1] = 0;
	shipmoveheading = 0;
	shipmoverange = 0;
	shipmovecount = 0;
	shipselectedheading = 0;
	alive = true;
	ship = false;
	quad = false;
	dirty = false;
}


OBJECT::~OBJECT()
{
	//mesh.~RENDERABLECOMPONENT();
}

void OBJECT::inittridebug(const void* pVSShaderByteCode, size_t vssize, const void* pPSShaderByteCode, size_t pssize, SIMPLE_VERTEX* verts, unsigned int vertcount, unsigned int* indices, unsigned int indicescount)
{
	topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	modelvertcount = vertcount;
	modelindexcount = indicescount;
	realtimemodel = verts;
	pvertindices = indices;
	vsbytecode = pVSShaderByteCode;
	psbytecode = pPSShaderByteCode;
	this->vssize = vssize;
	this->pssize = pssize;
}
void OBJECT::initobj(const void* pVSShaderByteCode, size_t vssize, const void* pPSShaderByteCode, size_t pssize, char* FileName)
{
	vector<vert_pos_skinned> verts;
	vector<unsigned int> indices;
	//loadOBJ(FileName, verts, indices);
	//
	vector<unsigned int> vertexindices, uvindices, normalindices;
	vector<XMFLOAT4>vertices;
	vector<XMFLOAT2>uvs;
	vector<XMFLOAT4>normals;
	FILE* file;
	fopen_s(&file, FileName, "r");
	if (file == nullptr)
	{
		OutputDebugString(L"File is nullptr");
		return;
	}
	while (true)
	{
		char lineheader[128]{};
		int res = fscanf(file, "%s", lineheader);
		if (res == EOF)
		{
			break;
		}
		if (strcmp(lineheader, "v") == 0)
		{
			XMFLOAT4 vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertex.w = 1;
			vertices.push_back(vertex);
		}
		else if (strcmp(lineheader, "vt") == 0)
		{
			XMFLOAT2 uv;
			ZeroMemory(&uv, sizeof(XMFLOAT2));
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			uvs.push_back(uv);
		}
		else if (strcmp(lineheader, "vn") == 0)
		{
			XMFLOAT4 normal;
			ZeroMemory(&normal, sizeof(XMFLOAT4));
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			normals.push_back(normal);
		}
		else if (strcmp(lineheader, "f") == 0)
		{
			unsigned int vertexindex[3], uvindex[3], normalindex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexindex[0], &uvindex[0], &normalindex[0], &vertexindex[1], &uvindex[1], &normalindex[1], &vertexindex[2], &uvindex[2], &normalindex[2]);
			//add this error check to the vert uv and normal fscanf
			if (matches != 9)
			{
				OutputDebugString(L"file corrupt or just can not be read");
				return;
			}
			vertexindices.push_back(vertexindex[0]);
			vertexindices.push_back(vertexindex[1]);
			vertexindices.push_back(vertexindex[2]);
			uvindices.push_back(uvindex[0]);
			uvindices.push_back(uvindex[1]);
			uvindices.push_back(uvindex[2]);
			normalindices.push_back(normalindex[0]);
			normalindices.push_back(normalindex[1]);
			normalindices.push_back(normalindex[2]);
		}
	}
	for (size_t i = 0; i < vertexindices.size(); i++)
	{
		vert_pos_skinned vertex;
		unsigned int vertexindex = vertexindices[i];
		vertex.pos = vertices[vertexindex - 1];
		unsigned int uvindex = uvindices[i];
		vertex.uv = uvs[uvindex - 1];
		unsigned int normalindex = normalindices[i];
		vertex.norm = normals[normalindex - 1];
		//FileMesh.push_back(vertex);
		verts.push_back(vertex);
		//VertIndices.push_back((unsigned int)i);
		indices.push_back((unsigned int)i);
	}
	/////////////////////////////////////////
	topology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	modelvertcount = (unsigned int)verts.size();
	modelindexcount = (unsigned int)indices.size();
	realtimemodel = new SIMPLE_VERTEX[modelvertcount];
	XMFLOAT4 color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	for (size_t i = 0; i < modelvertcount; i++)
	{
		realtimemodel[i].xyzw = verts[i].pos;
		realtimemodel[i].normal = verts[i].norm;
		realtimemodel[i].uv = verts[i].uv;
		realtimemodel[i].color = color;
	}
	pvertindices = new unsigned int[modelindexcount];
	for (size_t i = 0; i < modelindexcount; i++)
	{
		pvertindices[i] = indices[i];
	}
	vsbytecode = pVSShaderByteCode;
	psbytecode = pPSShaderByteCode;
	this->vssize = vssize;
	this->pssize = pssize;


}