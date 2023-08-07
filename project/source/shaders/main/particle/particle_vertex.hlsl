#include "../../camera_constant_buffer.hlsli"

struct VS_INPUT
{
    float4 color : INSTANCECOLOR;
    float3 position : INSTANCEPOSITION;
    float rotationAngle : INSTANCEROTATIONANGLE;
    float2 size : INSTANCESIZE;
    float time : INSTANCETIME;
    uint vertexID : SV_VertexID;
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

static const float2 offset[4] =
{
    float2(-1.0, 1.0), // Top-left vertex
    float2(1.0, 1.0), //  Top-right vertex
    float2(-1.0, -1.0), // Bottom-left vertex 
    float2(1.0, -1.0), // Bottom-right vertex
};

VS_OUTPUT
main
(VS_INPUT input)
{
    const float2 offsetID = offset[input.vertexID];
    
    const float cosAngle = cos(input.rotationAngle);
    const float sinAngle = sin(input.rotationAngle);
    const float2x2 rotationMat = float2x2(
        cosAngle, -sinAngle,
        sinAngle, cosAngle);
    
    const float2 rotatedOffsetID = mul(rotationMat, offsetID);
    const float2 resizedOffsetID = rotatedOffsetID * input.size;
    
    const float3 worldPosition = input.position + resizedOffsetID.x * g_cameraHorizontal + resizedOffsetID.y * g_cameraVertical;
    const float4 viewPosition = mul(g_worldToView, float4(worldPosition, 1.f));
    const float4 clipPosition = mul(g_viewToClip, viewPosition);
    
    const float2 textureUV = float2(
        offsetID.x * 0.5f + 0.5f,
        offsetID.y * -0.5f + 0.5f);
    
    VS_OUTPUT output;
	{
        output.worldPosition = worldPosition;
        output.position = clipPosition;
        output.textureUV = textureUV;
        output.screenUV = float2(
            (output.position.x / output.position.w) * 0.5f + 0.5f,
            (output.position.y / output.position.w) * -0.5f + 0.5f);
        output.color = input.color;
        output.time = input.time;
    }
    return output;
}
