#include "../../global_samplers_states.hlsli"
#include "../../light_constant_buffer.hlsli"
#include "../../camera_constant_buffer.hlsli"
#include "../lights_pbr.hlsli"
#include "../spotlight.hlsli"

Texture2D g_depthTexture : register(t13);
Texture2D g_textureRLU : register(t14);
Texture2D g_textureDBF : register(t15);
Texture2D g_textureEMVA : register(t16);
Texture2D g_flashlightMask : register(t11);

cbuffer EmmiterCBS : register(b8)
{
    uint g_framesVerticalNum;
    uint3 _emmiter_cbs_pad1;
    uint g_framesHorizontalNum;
    uint3 _emmiter_cbs_pad2;
    float g_particleLifeTime;
    float3 _emmiter_cbs_pad3;
};

struct VS_OUTPUT
{
    float4 color : INSTANCECOLOR;
    float2 screenUV : SCREENUV;
    float2 textureUV : TEXCOORD;
    float3 worldPosition : WORLDPOSITION;
    float4 position : SV_Position;
    float time : INSTANCETIME;
};

struct PS_OUTPUT
{
    float4 color : SV_TARGET;
};

static const float THICKNESS = 1.f;
static const float INV_THICKNESS = 1.f / THICKNESS;

static const float FRAME_LIFETIME = g_particleLifeTime / (g_framesVerticalNum * g_framesHorizontalNum);
static const float FRAME_DX = 1.f / g_framesHorizontalNum;
static const float FRAME_DY = 1.f / g_framesVerticalNum;
static const float2 FRAME_DXY = float2(FRAME_DX, FRAME_DY);

void caculateMotionLightmaps(float frameTime, float2 normilizeUV, out float3 lightmapRLU, out float3 lightmapDBF, out float2 emissionAlpha);
float computeLightMapFactor(float3 dir, float3 lightMapRLU, float3 lightMapDBF);
float2 getFrameUV(int frame);

float3 comput6WayPerLightRadiance(
    const float3 lightDirection,
    const float lightSolidAngle,
    const float3 lightIntensity,
    const float3 lightmapRLU,
    const float3 lightmapDBF);

PS_OUTPUT
main
(VS_OUTPUT input)
{
    float3 lightmapRLU;
    float3 lightmapDBF;
    float2 emissionAlpha;
    caculateMotionLightmaps(input.time / FRAME_LIFETIME, input.textureUV, lightmapRLU, lightmapDBF, emissionAlpha);

    const float3 vertex = input.worldPosition;
    const float3 view = normalize(g_cameraPosition - vertex);
    
    float3 radiance = 0.f;
    for (int i = 0; i < g_pointLighsSize; ++i)
    {
        float3 direction = normalize(g_pointLights[i].position - vertex);
        float3 intensity = g_pointLights[i].intensity;
        float solidAngle = calculateSolidAngle(g_pointLights[i].position, g_pointLights[i].radius * g_pointLights[i].radius, vertex);
        radiance += comput6WayPerLightRadiance(direction, solidAngle, intensity, lightmapDBF, lightmapDBF);
    }
    
    for (int i = 0; i < g_spotLighsSize; ++i)
    {
        float3 direction = normalize(g_spotLights[i].position - vertex);
        float3 intensity = spotLightIntensityMask(g_spotLights[i], vertex, g_flashlightMask);
        float solidAngle = calculateSolidAngle(g_spotLights[i].position, g_spotLights[i].radius * g_spotLights[i].radius, vertex);
        radiance += comput6WayPerLightRadiance(direction, solidAngle, intensity, lightmapDBF, lightmapDBF);
    }
    
    for (int i = 0; i < g_directionalLighsSize; ++i)
    {
        radiance += comput6WayPerLightRadiance(g_directionalLights[i].direction, g_directionalLights[i].solidAngle, g_directionalLights[i].intensity, lightmapDBF, lightmapDBF);
    }
    const float particleDepth = input.position.z;
    const float sceneDepth = g_depthTexture.Sample(g_sampler, input.screenUV).r;
    const float depthDiff = 1.f / sceneDepth - 1.f / particleDepth;
    const float currentAlpha = input.color.a;
    const float smoothClippingAlpha = depthDiff < THICKNESS ? (max(currentAlpha - (THICKNESS - depthDiff) * INV_THICKNESS, 0.f)) : (currentAlpha);
    const float resAlpha = emissionAlpha.y * smoothClippingAlpha;
    PS_OUTPUT output;
    output.color = float4(radiance * input.color.rgb, resAlpha);
    return output;

}

void caculateMotionLightmaps(float frameTime, float2 normilizeUV, out float3 lightmapRLU, out float3 lightmapDBF, out float2 emissionAlpha)
{
    int frame = int(frameTime);
    
    float2 uvThis = getFrameUV(frame) + normilizeUV * FRAME_DXY; // compute motion-vector sample uv for the current frame
    float2 uvNext = getFrameUV(frame + 1) + normilizeUV * FRAME_DXY; // !TODO compute motion-vector sample uv for the next frame

    // ----------- sample motion-vectors -----------
    float2 mv0 = 2.0 * g_textureEMVA.Sample(g_sampler, uvThis).gb - 1.0; // current frame motion-vector
    float2 mv1 = 2.0 * g_textureEMVA.Sample(g_sampler, uvNext).gb - 1.0; // next frame motion-vector

    // need to flip motion-vector Y specifically for the smoke textures:
    mv0.y = -mv0.y;
    mv1.y = -mv1.y;

    // ----------- UV flowing along motion-vectors -----------
    static const float MV_SCALE = 0.0015; // adjusted for the smoke textures
    
    
    float time = frameTime - frame; // goes from 0.0 to 1.0 between two sequential frames

    float2 uv0 = uvThis; // texture sample uv for the current frame
    uv0 -= mv0 * MV_SCALE * time; // if MV points in some direction, then UV flows in opposite

    float2 uv1 = uvNext; // texture sample uv for the next frame
    uv1 -= mv1 * MV_SCALE * (time - 1.f); // if MV points in some direction, then UV flows in opposite

    // ----------- sample textures -----------
    float2 emissionAlpha0 = g_textureEMVA.Sample(g_sampler, uv0).ra;
    float2 emissionAlpha1 = g_textureEMVA.Sample(g_sampler, uv1).ra;

    // .x - right, .y - left, .z - up
    float3 lightmapRLU0 = g_textureRLU.Sample(g_sampler, uv0).rgb;
    float3 lightmapRLU1 = g_textureRLU.Sample(g_sampler, uv1).rgb;

    // .x - down, .y - back, .z - front
    float3 lightmapDBF0 = g_textureDBF.Sample(g_sampler, uv0).rgb;
    float3 lightmapDBF1 = g_textureDBF.Sample(g_sampler, uv1).rgb;

    // ----------- lerp values -----------
    emissionAlpha = lerp(emissionAlpha0, emissionAlpha1, time);
    lightmapRLU = lerp(lightmapRLU0, lightmapRLU1, time);
    lightmapDBF = lerp(lightmapDBF0, lightmapDBF1, time);
}

float computeLightMapFactor(float3 dir, float3 lightMapRLU, float3 lightMapDBF)
{
    const float hMap = (dir.x > 0.0f) ? (lightMapRLU.r) : (lightMapRLU.g);
    const float vMap = (dir.y > 0.0f) ? (lightMapRLU.b) : (lightMapDBF.r);
    const float dMap = (dir.z > 0.0f) ? (lightMapDBF.b) : (lightMapDBF.g);
    const float lightMap = hMap * dir.x * dir.x + vMap * dir.y * dir.y + dMap * dir.z * dir.z;
    return lightMap;
}

float2 getFrameUV(int frame)
{
    float2 uv;
    uv.x = FRAME_DX * int(frame % g_framesHorizontalNum);
    uv.y = FRAME_DY * int(int(frame / g_framesVerticalNum) % g_framesVerticalNum);
    return uv;
}

float3 comput6WayPerLightRadiance(
    const float3 lightDirection, 
    const float lightSolidAngle, 
    const float3 lightIntensity, 
    const float3 lightmapRLU, 
    const float3 lightmapDBF)
{
    static const float3 DIRECTIONS[6] =
    {
        float3(1.f, 0.f, 0.f),
        float3(-1.f, 0.f, 0.f),
        float3(0.f, 1.f, 0.f),
        float3(0.f, -1.f, 0.f),
        float3(0.f, 0.f, 1.f),
        float3(0.f, 0.f, -1.f)
    };
    
    float3 radiance = 0.f;
    for (int j = 0; j < 6; ++j)
        radiance += lightIntensity * lightSolidAngle * computeLightMapFactor(DIRECTIONS[j], lightmapRLU, lightmapDBF) * max(0.f, dot(lightDirection, DIRECTIONS[j]));
    return radiance;
}