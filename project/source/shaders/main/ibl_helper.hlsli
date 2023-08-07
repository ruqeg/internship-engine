#ifndef IBL_HELPER_HLSLI
#define IBL_HELPER_HLSLI

#include "../global_samplers_states.hlsli"
#include "textures_declaration.hlsli"

float3 E_diffuseIrradiance(const float3 n)
{
    return g_diffuseIrradianceCubemap.SampleLevel(g_sampler, n, 0u);
}

float3 E_specularIrradiance(const float3 n, const float3 v, const float r)
{
    uint width, height, numberOfLevels;
    g_specularIrradianceCubemap.GetDimensions(0, width, height, numberOfLevels);
    return g_specularIrradianceCubemap.SampleLevel(g_sampler, reflect(-v, n), numberOfLevels * r);
}

float2 E_specularReflectance(const float3 n, const float3 v, const float r)
{
    float2 uv = float2(max(dot(n, v), 0.f), r);
    return g_specularReflectanceTexture2D.SampleLevel(g_sampler, uv, 0u).rg;
}

#endif //IBL_HELPER_HLSLI