#ifndef LIGHT_CONSTANT_BUFFER_HLSLI
#define LIGHT_CONSTANT_BUFFER_HLSLI

struct DirectionalLightsCBS
{
    float3 direction;
    float _pad1;
    float3 intensity;
    float _pad2;
    float solidAngle;
    float3 _pad3;
};

struct PointLightsCBS
{
    float3 position;
    float _pad1;
    float3 intensity;
    float _pad2;
    float radius;
    float3 _pad3;
};

struct SpotLightsCBS
{
    float3 position;
    float _pad1;
    float3 intensity;
    float _pad2;
    float radius;
    float3 _pad3;
    float3 direction;
    float _pad4;
    float angle;
    float3 _pad5;
};

static const int maxCount = 4;

cbuffer LightCB : register(b4)
{
    DirectionalLightsCBS g_directionalLights[maxCount];
    PointLightsCBS g_pointLights[maxCount];
    SpotLightsCBS g_spotLights[maxCount];
    uint g_pointLighsSize;
    uint3 _pad1;
    uint g_directionalLighsSize;
    uint3 _pad2;
    uint g_spotLighsSize;
    uint3 _pad3;
};

#endif //LIGHT_CONSTANT_BUFFER_HLSLI