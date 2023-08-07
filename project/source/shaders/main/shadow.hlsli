#ifndef MAIN_SHADOW_HLSLI
#define MAIN_SHADOW_HLSLI

#include "../global_samplers_states.hlsli"
#include "textures_declaration.hlsli"
#include "cbuffer_declaration.hlsli"

static const float STATIC_DEPTH_BIAS = -2.f;
static const float SCALED_DEPTH_BIAS = -2.f;

float shadowCalculation2D(float4 lightClipPosition, Texture2D shadowMap2D, float gndotl)
{
    // perform perspective divide
    float3 projCoords = lightClipPosition.xyz / lightClipPosition.w;
    // transform to [0,1] range
    float2 sampleCoords;
    sampleCoords.x = projCoords.x * 0.5 + 0.5;
    sampleCoords.y = -projCoords.y * 0.5 + 0.5;
    
    if (sampleCoords.x < 0 || sampleCoords.y < 0 || sampleCoords.x > 1 || sampleCoords.y > 1)
        return 1.f;
    
    uint w, h, element;
    shadowMap2D.GetDimensions(w, h);
    float xoffset = 0.5f / w;
    float yoffset = 0.5f / h;
    
    const float depth = projCoords.z;
    const float staticBias = STATIC_DEPTH_BIAS;
    const float scaledBias = SCALED_DEPTH_BIAS * max(0.05 * (1.0 - gndotl), 0.005);
    const float bias = staticBias + scaledBias;
    const float depthWithBias = depth / (1 + bias * depth);
    // final depthWithBias = 1.f / (1.f / depth + bias);

    float v = 0.f;
    v += shadowMap2D.SampleCmp(g_sampCmpState, sampleCoords.xy, depthWithBias);
    v += shadowMap2D.SampleCmp(g_sampCmpState, sampleCoords.xy + float2(xoffset, 0.0f), depthWithBias);
    v += shadowMap2D.SampleCmp(g_sampCmpState, sampleCoords.xy + float2(-xoffset, 0.0f), depthWithBias);
    v += shadowMap2D.SampleCmp(g_sampCmpState, sampleCoords.xy + float2(0.0f, yoffset), depthWithBias);
    v += shadowMap2D.SampleCmp(g_sampCmpState, sampleCoords.xy + float2(0.0f, -yoffset), depthWithBias);
    
    v /= 5.f;
    return smoothstep(0.33f, 1.f, v);
}

float spotlightShadowCalculation(float3 worldPos, uint index, float gndotl)
{
    const float4 lightViewPosition = mul(g_shadowMaps2DCBs[4 + index].worldToLightView, float4(worldPos, 1.f));
    const float4 lightClipPosition = mul(g_shadowMaps2DCBs[4 + index].lightViewToClip, lightViewPosition);
    const float shadow = shadowCalculation2D(lightClipPosition, g_shadowMaps2D[4 + index], gndotl);
    return shadow;
}

float pointlightShadowCalculation(float3 worldPos, uint index, float gndotl)
{
    float3 dir = normalize(worldPos - g_shadowMaps3DCBs[index].lightPosition);
    
    float maxAbsDirV = max(abs(dir.x), max(abs(dir.y), abs(dir.z)));
    uint face;
    if (maxAbsDirV == dir.x)
        face = 0;
    else if (maxAbsDirV == -dir.x)
        face = 1;
    else if (maxAbsDirV == dir.y)
        face = 2;
    else if (maxAbsDirV == -dir.y)
        face = 3;
    else if (maxAbsDirV == dir.z)
        face = 4;
    else
        face = 5;
    
    float4 viewPos = mul(g_shadowMaps3DCBs[index].worldToLightViewsPerFace[face], float4(worldPos, 1.f));
    float4 clipPos = mul(g_shadowMaps3DCBs[index].lightViewToClip, viewPos);
    
    float3 projCoords = clipPos.xyz / clipPos.w;
    
    const float depth = projCoords.z;
    const float staticBias = STATIC_DEPTH_BIAS;
    const float scaledBias = SCALED_DEPTH_BIAS * max(0.05 * (1.0 - gndotl), 0.005);
    const float bias = staticBias + scaledBias;
    const float depthWithBias = depth / (1 + bias * depth);
    
    float shadowFactor = g_shadowMapsCubemaps[index].SampleCmpLevelZero(g_sampCmpState, dir, depthWithBias).r;
    return shadowFactor;
}

float directionallightShadowCalculation(float3 worldPos, uint index, float gndotl)
{
    const float4 lightViewPosition = mul(g_shadowMaps2DCBs[index].worldToLightView, float4(worldPos, 1.f));
    const float4 lightClipPosition = mul(g_shadowMaps2DCBs[index].lightViewToClip, lightViewPosition);
    const float shadow = shadowCalculation2D(lightClipPosition, g_shadowMaps2D[index], gndotl);
    return shadow;
}

#endif //MAIN_SHADOW_HLSLI