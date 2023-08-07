TextureCube g_texture : register(t10);

#include "../global_samplers_states.hlsli"

struct VS_OUTPUT
{
	float4 inPosition : SV_POSITION;
	float3 inFustumCornerVec : DIRECTION;
};

float4 main(VS_OUTPUT input) : SV_TARGET
{
	float3 pixelColor = g_texture.Sample(g_sampler, input.inFustumCornerVec.xyz);
	return float4(pixelColor, 1.0f);
}
