#include "../../camera_constant_buffer.hlsli"
#include "../../mesh_constant_buffer.hlsli"

struct VS_INPUT
{
    float3 inPosition : POSITION;
    float2 inUV : TEXCOORD;
    float3 inNormal : NORMAL;
    float3 inTangent : TANGENT;
    float3 inBitangent : BITANGENT;
    float4 decalToWorld1 : INSTANCEDECALTOWORLD1N;
    float4 decalToWorld2 : INSTANCEDECALTOWORLD2N;
    float4 decalToWorld3 : INSTANCEDECALTOWORLD3N;
    float4 decalToWorld4 : INSTANCEDECALTOWORLD4N;
    float4 worldToDecal1 : INSTANCEWORLDTODECAL1N;
    float4 worldToDecal2 : INSTANCEWORLDTODECAL2N;
    float4 worldToDecal3 : INSTANCEWORLDTODECAL3N;
    float4 worldToDecal4 : INSTANCEWORLDTODECAL4N;
    float4 color : INSTANCECOLOR;
    uint parentObjectID : INSTANCEPARENTOBJECTID;
};

struct VS_OUTPUT
{
    float4 position : SV_POSITION;
    float4x4 decalToWorld : decalToWorld;
    float4x4 worldToDecal : worldToDecal;
    float4 color : INSTANCECOLOR;
    uint parentObjectID : INSTANCEPARENTOBJECTID;
};

VS_OUTPUT
main
(VS_INPUT input)
{
    const float4x4 decalToWorld = float4x4(input.decalToWorld1, input.decalToWorld2, input.decalToWorld3, input.decalToWorld4);
    const float4x4 worldToDecal = float4x4(input.worldToDecal1, input.worldToDecal2, input.worldToDecal3, input.worldToDecal4);
    const float4 meshPosition = float4(input.inPosition, 1.0f);
    const float4 modelPosition = mul(g_meshToModel, meshPosition);
    const float4 worldPosition = mul(decalToWorld, modelPosition);
    const float4 viewPosition = mul(g_worldToView, worldPosition);
    const float4 clipPosition = mul(g_viewToClip, viewPosition);

    VS_OUTPUT output;
	{
        output.position = clipPosition;
        output.decalToWorld = decalToWorld;
        output.worldToDecal = worldToDecal;
        output.color = input.color;
        output.parentObjectID = input.parentObjectID;
    }
    return output;
}