struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float2 outUV : UV;
};

static const int SAMPLE_COUNT = 100;
static const float epsilon = 0.001f;

#include "../math_constants.hlsli"
#include "../bdrf/g/ggx_smith.hlsli"
#include "help_fun.hlsli"

float4 main(VS_OUTPUT input) : SV_TARGET
{
    const float roughness = input.outUV.x;
    const float NdotV = input.outUV.y;
    
    const float3 n = float3(0.f, 0.f, 1.f);
    
    const float roughness2 = roughness * roughness;
    const float roughness4 = roughness2 * roughness2;
 
    float K_specR = 0.f;
    float K_specG = 0.f;
    
    for (int i = 0; i < SAMPLE_COUNT; ++i)
    {
        const float2 random = randomHammersley(i, SAMPLE_COUNT);
        const float3 h = randomGGX(random, roughness4);
        const float3 v = float3(sqrt(1.f - NdotV * NdotV), 0.f, NdotV);
        
        const float HdotV = dot(h, v);
        
        const float3 l = normalize(2.f * HdotV * h - v);
        
        const float NdotL = l.z;
        const float NdotH = h.z;

        if (NdotL > epsilon)
        {
            const float GGXsmith = G(roughness4, NdotV, NdotL);
            const float G_HdotV = GGXsmith * HdotV;
            
            const float M_HdotV_5 = pow(1.f - HdotV, 5);
            const float inv_NdotV_NdotH = 1.f / (NdotV * NdotH);
        
            K_specR += G_HdotV * (1.f - M_HdotV_5) * inv_NdotV_NdotH;
            K_specG += G_HdotV * M_HdotV_5 * inv_NdotV_NdotH;

        }
    }
    K_specR /= SAMPLE_COUNT;
    K_specG /= SAMPLE_COUNT;

    return float4(K_specR, K_specG, 0.f, 1.f);
}