struct INPUT_VERTEX
{
	float4 coordinate : POSITION;
	float4 normal : NORMAL;
	float4 color : COLOR;
	float2 uv : UV;
	float4 index : INDEX;
	float4 weight : WEIGHT;
};

struct OUTPUT_VERTEX
{
	float4 colorOut : COLOR;
	float4 projectedCoordinate : SV_POSITION;
	float4 l_pos : COLOR1;
	float4 normal : NORMAL;
	float2 uv : UV;
};

cbuffer THIS_IS_VRAM : register(b0)
{
	matrix camView;
	matrix camProj;
	float3 spot_light_pos;
	float3 spot_light_dir;
};

cbuffer THIS_IS_ANIMATION_VRAM : register(b1)
{
	//matrix inverseBindPose[128];
	matrix modelPos;
	matrix realTimePose[128];
};

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	float4 pos = float4(fromVertexBuffer.coordinate.xyzw);

	//pos.w = 1.0f;
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;
	pos = mul(pos, modelPos);
	sendToRasterizer.l_pos = pos;
	pos = mul(pos, camView);
	pos = mul(pos, camProj);

	float4 nor = normalize(fromVertexBuffer.normal);
	nor = mul(nor, modelPos);

	sendToRasterizer.colorOut = fromVertexBuffer.color;
	sendToRasterizer.projectedCoordinate = pos;
	sendToRasterizer.normal = nor;
	sendToRasterizer.uv = fromVertexBuffer.uv;

	return sendToRasterizer;
}