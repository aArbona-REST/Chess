Texture2D Texture : register(t0);
SamplerState DefaultFilter : register (s0);
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
	float4 spot_light_pos;
	float4 spot_light_dir;
};
float4 main(OUTPUT_VERTEX input) : SV_TARGET
{
	float4 Texture_Color = Texture.Sample(DefaultFilter, input.uv);
	return input.colorOut * Texture_Color;



	//light color
	float4 Light_Color = float4(1.0f, 1.0f, 1.0f, 0.0f);


	//point light//
	//float4 Light_Direction = normalize(spot_light_pos - input.l_pos);
	//float Light_Ratio = clamp(dot(spot_light_dir, normalize(input.normal)), 0.0f, 1.0f);
	//float p_lightAttenuation = 1.0f - clamp(length(spot_light_pos - input.l_pos) / 200.0f, 0.0f, 1.0f);
	//Light_Color = p_lightAttenuation * Light_Ratio * Light_Color;

	//spot lighting//
	//float3 sld = spot_light_dir.xyz;
	//float3 s_light_Direction = normalize(slp - input.l_pos.xyz);
	//float s_surfaceRatio = clamp(dot(-s_light_Direction, sld), 0.0f, 1.0f);
	//float s_lightRatio = clamp(dot(s_light_Direction, input.normal.xyz), 0.0f, 1.0f);
	//float s_lightDistanceAttenuation = 1.0f - clamp(length(spot_light_pos.xyz - input.l_pos.xyz) / 10.0f, 0.0f, 1.0f);
	//float s_lightRadiusAttenuation = 1.0f - clamp((0.9f - s_surfaceRatio) / (0.9f - 0.5f), 0.0f, 1.0f);
	//Light_Color = s_lightRadiusAttenuation * s_lightDistanceAttenuation * s_lightRatio * Light_Color;

	//return saturate(input.colorOut * Texture_Color * Light_Color);
}