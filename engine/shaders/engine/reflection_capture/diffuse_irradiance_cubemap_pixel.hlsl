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

#include "../math_constants.hlsli"
#include "../bdrf/f/fresnel_law.hlsli"
#include "help_fun.hlsli"

static const int SAMPLE_COUNT = 100000;

PS_OUTPUT main(VS_OUTPUT input)
{
    PS_OUTPUT output;
    
    float3 irradiance = 0;
    const float3x3 mat = basisFromDir(input.direction);
    int width, height;
    g_skyboxTexture.GetDimensions(width, height);
    const float mipmapLevel = hemisphereMip(1 / SAMPLE_COUNT, width);
    
    for (int i = 0; i < SAMPLE_COUNT; ++i)
    {
        float ndotv;
        float3 sampleDir = randomHemisphere(ndotv, i, SAMPLE_COUNT);
        float3 rotatedSampleDir = mul(mat, sampleDir);
        float3 ek = g_skyboxTexture.SampleLevel(g_sampler, rotatedSampleDir, mipmapLevel);
        irradiance += (ek * ndotv) / PI * (1.f - F(ndotv, float3(0.4f, 0.4f, 0.4f)));

    }
    irradiance = 2 * PI * (irradiance / SAMPLE_COUNT);
    output.color = float4(irradiance, 1.f);
    return output;
}