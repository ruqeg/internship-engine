#include "../../global_samplers_states.hlsli"

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 screenUV : SCREENUV;
};

struct PS_OUTPUT
{
    float4 color : SV_Target;
    float depth : SV_Depth;
};

#define EMISSION_TEXTURE_INDEX t3
#define DEPTH_TEXTURE_INDEX t4

Texture2D g_emissionTexture : register(EMISSION_TEXTURE_INDEX);
Texture2D g_depthTexture : register(DEPTH_TEXTURE_INDEX);

PS_OUTPUT
main
(VS_OUTPUT input)
{
    const float4 emission = g_emissionTexture.Sample(g_sampler, input.screenUV);
    const float depth = g_depthTexture.Sample(g_sampler, input.screenUV).r;
    
    PS_OUTPUT output;
    output.color = emission;
    output.depth = depth;
    return output;
}