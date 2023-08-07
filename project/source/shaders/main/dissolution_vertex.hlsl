#include "../camera_constant_buffer.hlsli"
#include "../mesh_constant_buffer.hlsli"

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
    float modelTime : INSTANCETIME;
};

struct VS_OUTPUT
{
    float3 vWorldPosition : WORLDPOSITON;
    float4 vPosition : SV_POSITION;
    float2 vUV : TEXCOORD;
    float3 vNormal : NORMAL;
    float3 vTangent : TANGENT;
    float3 vBitangent : BITANGENT;
    float4 vColor : INSTANCECOLOR;
    float vTime : INSTANCETIME;
};

VS_OUTPUT
main
(VS_INPUT input)
{
    const float4x4 modelToWorld = float4x4(input.modelToWorld1, input.modelToWorld2, input.modelToWorld3, input.modelToWorld4);
    const float4 meshPosition = float4(input.inPosition, 1.0f);
    const float4 modelPosition = mul(g_meshToModel, meshPosition);
    const float4 worldPosition = mul(modelToWorld, modelPosition);
    const float4 viewPosition = mul(g_worldToView, worldPosition);
    const float4 clipPosition = mul(g_viewToClip, viewPosition);
    
    VS_OUTPUT output;
	{
        output.vWorldPosition = worldPosition;
        output.vPosition = clipPosition;
        output.vNormal = mul(modelToWorld, float4(input.inNormal, 0.0f));
        output.vColor = input.modelColor;
        output.vBitangent = mul(modelToWorld, float4(input.inBitangent, 0.0f));
        output.vTangent = mul(modelToWorld, float4(input.inTangent, 0.0f));
        output.vUV = input.inUV;
        output.vTime = input.modelTime;

    }
    return output;
}