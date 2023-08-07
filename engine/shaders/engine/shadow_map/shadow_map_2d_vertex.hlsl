cbuffer MeshConstantBuffer : register(b2)
{
    float4x4 g_meshToModel;
};

cbuffer ShadowMap2DConstantBuffer : register(b10)
{
    float4x4 g_worldToLightView;
    float4x4 g_lightViewToClip;
    float g_texelWorldSize;
    float3 _pad1;
    float3 g_lightPosition;
    float _pad2;
};

struct VS_INPUT
{
    float3 inPosition : POSITION;
    float2 inUV : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBitangent : BITANGENT;
    float4 modelToWorld1 : INSTANCEMODELTOWORLD1N;
    float4 modelToWorld2 : INSTANCEMODELTOWORLD2N;
    float4 modelToWorld3 : INSTANCEMODELTOWORLD3N;
    float4 modelToWorld4 : INSTANCEMODELTOWORLD4N;
    float4 modelColor : INSTANCECOLOR;
};

struct VS_OUTPUT
{
    float4 vPosition : SV_POSITION;
};

VS_OUTPUT
main
(VS_INPUT input)
{
    const float4x4 modelToWorld = float4x4(input.modelToWorld1, input.modelToWorld2, input.modelToWorld3, input.modelToWorld4);
    const float4 meshPosition = float4(input.inPosition, 1.0f);
    const float4 modelPosition = mul(g_meshToModel, meshPosition);
    const float4 worldPosition = mul(modelToWorld, modelPosition);
    const float3 normal = mul(modelToWorld, float4(input.inNormal, 0.0f));
    const float3 lighDir = normalize(g_lightPosition - worldPosition.xyz);
    const float3 offset = (g_texelWorldSize / sqrt(2.f)) * (normal - 0.9f * lighDir * dot(normal, lighDir));
    const float4 viewPosition = mul(g_worldToLightView, worldPosition - float4(offset, 0.f));
    const float4 clipPosition = mul(g_lightViewToClip, viewPosition);
    VS_OUTPUT output;
	{
        output.vPosition = clipPosition;
    }
    return output;
}
