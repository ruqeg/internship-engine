#include "postprocces_vertex_pixel_s.hlsli"
#include "aces_fitted.hlsli"

SamplerState g_samplerLinear : register(s0);
Texture2D g_viewBuffer : register(t0);

cbuffer PostProccesCBS : register(b0)
{
    float g_gamma;
    float3 _pad1;
    float g_EV100;
    float3 _pad2;
};

struct PS_OTPUT
{
    float4 color: SV_TARGET;
};

float3 adjustExposure(float3 lightPower, float EV100)
{
    const float LMax = (78.0f / (0.65f * 100.0f)) * pow(2.0f, EV100);
    return lightPower * (1.0f / LMax);
}

float3 correctGamma(float3 color, float gamma)
{
    return pow(color, 1.0f / gamma);
}

PS_OTPUT
main
(VS_OUTPUT input)
{   
    const float3 lightPower = g_viewBuffer.Sample(g_samplerLinear, input.outUV);
    const float3 exposureAdjustedLightPower = adjustExposure(lightPower, g_EV100);
    const float3 ldrColor = ACESFitted(exposureAdjustedLightPower);
    const float3 gammaCorrectedLdrColor = correctGamma(ldrColor, g_gamma);
    float luma = dot(gammaCorrectedLdrColor, float3(0.2126f, 0.7512f, 0.0722f));
    PS_OTPUT output;
    {
        output.color = float4(gammaCorrectedLdrColor, luma);
    }
    return output;
}