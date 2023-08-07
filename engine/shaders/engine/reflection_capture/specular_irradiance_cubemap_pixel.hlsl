TextureCube g_skyboxTexture : register(t0);
SamplerState g_sampler : register(s0);

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float3 direction : DIRECTION;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

cbuffer SpecularIrradianceCB : register(b1)
{
    float g_roughness;
};

#include "../math_constants.hlsli"
#include "../bdrf/f/fresnel_law.hlsli"
#include "../bdrf/d/trowbridge_reitz_ggx.hlsli"
#include "help_fun.hlsli"

static const int SAMPLE_COUNT = 5000;
static const float epsilon = 0.001f;

PS_OUTPUT main(VS_OUTPUT input)
{
    PS_OUTPUT output;
    
    const float3 n = input.direction;
    const float3 v = n;
    const float3x3 mat = basisFromDir(input.direction);
    int width, height;
    g_skyboxTexture.GetDimensions(width, height);

    const float roughness2 = g_roughness * g_roughness;
    const float roughness4 = roughness2 * roughness2;
    
    float3 irradiance = 0;
    
    for (int i = 0; i < SAMPLE_COUNT; ++i)
    {
        float vdoth;
        const float3 h = randomGGX(vdoth, i, SAMPLE_COUNT, roughness4, mat);
        const float3 l = normalize(2.f * vdoth * h - v);
        const float LdotN = dot(l, n);
        if (LdotN > epsilon)
        {
            //const uint s = 
            //const uint mipmap = 0.5f * log2(S * 3 * res2);
            const float mipmapLevel = hemisphereMip(4.f / (2.f * PI * D(roughness2, LdotN) * SAMPLE_COUNT), width);
            const float3 ek = g_skyboxTexture.SampleLevel(g_sampler, l, mipmapLevel);
            irradiance += ek * LdotN;
        }

    }
    irradiance /= SAMPLE_COUNT;
    output.color = float4(irradiance, 1.f);
    return output;
}