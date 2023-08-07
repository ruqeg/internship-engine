#include "vertex_output_struct.hlsli"
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
};

VS_OUTPUT 
main
(VS_INPUT input)
{
    const float4x4 modelToWorld = float4x4(input.modelToWorld1, input.modelToWorld2, input.modelToWorld3, input.modelToWorld4);
    const float4 meshPositon = float4(input.inPosition, 1.0f);
    const float4 modelPosition = mul(g_meshToModel, meshPositon);
    const float4 worldPosition = mul(modelToWorld, modelPosition);
    VS_OUTPUT output;
	{
        output.vWorldPosition = worldPosition;
        output.vNormal = input.inNormal;
        output.vColor = input.modelColor;
        output.vBitangent = input.inBitangent;
        output.vTangent = input.inTangent;
        output.vUV = input.inUV;
    }
	return output;
}
