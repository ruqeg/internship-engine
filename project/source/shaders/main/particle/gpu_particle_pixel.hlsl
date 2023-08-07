#include "../../global_samplers_states.hlsli"

Texture2D g_sparkTexture : register(t10);

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 textureUV : TEXCOORD;
    float3 color : PARTICELCOLOR;
};

struct PS_OUTPUT
{
    float4 color : SV_Target;
    float depth : SV_Depth;
};

PS_OUTPUT
main
(VS_OUTPUT input)
{
    PS_OUTPUT output;
    float mask = g_sparkTexture.Sample(g_sampler, input.textureUV).r;
    
    output.depth = (mask > 0.01f) ? input.position.z : 0.f;
    
    output.color = float4(input.color, mask);
    return output;
}