#ifndef CBUFFER_DECLARATION_HLSLI
#define CBUFFER_DECLARATION_HLSLI

cbuffer TextureViewCB : register(b2)
{
    float g_sampleStateIndex;
};

cbuffer IBLCB : register(b10)
{
    bool g_E_direct;
    float3 _IBLCB_pad1;
    bool g_E_diff;
    float3 _IBLCB_pad2;
    bool g_E_spec;
    float3 _IBLCB_pad3;
};

struct ShadowMap2DConstantBufferStruct
{
    float4x4 worldToLightView;
    float4x4 lightViewToClip;
    float texelWorldSize;
    float3 _pad1;
    float3 lightPosition;
    float _pad2;
};

struct ShadowMap3DConstantBufferStruct
{
    float4x4 worldToLightViewsPerFace[6];
    float4x4 lightViewToClip;
    float texelWorldSize;
    float3 _pad1;
    float3 lightPosition;
    float _pad2;
};

cbuffer ShadowMap2DArrayConstantBuffer : register(b12)
{
    ShadowMap2DConstantBufferStruct g_shadowMaps2DCBs[8];
    ShadowMap3DConstantBufferStruct g_shadowMaps3DCBs[8];
};

#endif //CBUFFER_DECLARATION_HLSLI