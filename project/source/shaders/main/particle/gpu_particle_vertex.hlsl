#include "../../camera_constant_buffer.hlsli"

struct ParticleGPUInstancec
{
    float3 position;
    float3 force;
    float3 color;
    float speed;
    float time;
};

#define PARTICLE_BUFFER_INDEX t0
#define RANGE_BUFFER_INDEX t1
StructuredBuffer<ParticleGPUInstancec> inputParticleBuffer : register(PARTICLE_BUFFER_INDEX);
Buffer<uint> g_inputRangeBuffer : register(RANGE_BUFFER_INDEX);

struct VS_INPUT
{
    uint vertexID : SV_VertexID;
    uint instanceID : SV_InstanceID;
};

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 textureUV : TEXCOORD;
    float3 color : PARTICELCOLOR;
};

static const float2 offset[4] =
{
    float2(-1.0, 1.0), // Top-left vertex
    float2(1.0, 1.0), //  Top-right vertex
    float2(-1.0, -1.0), // Bottom-left vertex 
    float2(1.0, -1.0), // Bottom-right vertex
};

static const float SIZE_SCALE = 1.f;

VS_OUTPUT
main
(VS_INPUT input)
{
    const float2 offsetID = offset[input.vertexID];
    const float2 resizedOffsetID = SIZE_SCALE * offsetID;
    
    const uint particleBufferOffset = g_inputRangeBuffer[1];
    const uint particleIndex = input.instanceID + particleBufferOffset;
    
    ParticleGPUInstancec particle = inputParticleBuffer[particleIndex];
    
    const float3 worldPosition = particle.position + resizedOffsetID.x * g_cameraHorizontal + resizedOffsetID.y * g_cameraVertical;
    const float4 viewPosition = mul(g_worldToView, float4(worldPosition, 1.f));
    const float4 clipPosition = mul(g_viewToClip, viewPosition);
    
    const float2 textureUV = float2(
        offsetID.x * 0.5f + 0.5f,
        offsetID.y * -0.5f + 0.5f);
    
    VS_OUTPUT output;
    output.position = clipPosition;
    output.textureUV = textureUV;
    output.color = particle.color;
    
    return output;
}
