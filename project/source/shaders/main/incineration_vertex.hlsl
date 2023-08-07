#include "../camera_constant_buffer.hlsli"
#include "../mesh_constant_buffer.hlsli"

cbuffer RangeBufferCB : register(b10)
{
    uint g_rangeBufferNumber;
    uint g_rangeBufferOffset;
    uint g_rangeBufferExpired;
    uint g_rangeBufferVertexCountPerInstance;
    uint g_rangeBufferInstanceCount;
    uint g_rangeBufferStartVertexLocation;
    uint g_rangeBufferStartInstanceLocation;
};

struct ParticleGPUInstancec
{
    float3 position;
    float3 force;
    float3 color;
    float speed;
    float time;
};

#define PARTICLE_BUFFER_INDEX u5
#define RANGE_BUFFER_INDEX u6
RWStructuredBuffer<ParticleGPUInstancec> outputParticleBuffer : register(PARTICLE_BUFFER_INDEX);
RWBuffer<uint> outputRangeBuffer : register(RANGE_BUFFER_INDEX);

struct VS_INPUT
{
    uint vertexID : SV_VertexID;
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
    float3 modelRayIntersection : RAYINTERSECTION;
    float modelTime : INSTANCETIME;
    float modelPrevBoundingSphereRadius : INSTANCEPREVBOUNDINGSHPERERADIUS;
    float modelCurrentBoundingSphereRadius : INSTANCECURRENTBOUNDINGSHPERERADIUS;
    float modelMaxBoundingSphereRadius : INSTANCEMAXBOUNDINGSHPERERADIUS;
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

static const float INIT_GPU_PARTICLE_SPEED = 1.0f;

static const uint MAX_PARTICLES = 100u;

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
    
    const float3 modelNormal = mul(modelToWorld, float4(input.inNormal, 0.0f));
    
    const float3 sphereCenter = input.modelRayIntersection;
    const float3 VO = worldPosition.xyz - sphereCenter; // vertex - shpere cetner (0)
    const float VO_dot = dot(VO, VO); //(x-x0)2 + (y-y0)2 + (z-z0)2
    const bool byteOk = (outputRangeBuffer[0] < MAX_PARTICLES) && (input.vertexID % 7 == 0) && (VO_dot > input.modelPrevBoundingSphereRadius) && (VO_dot < input.modelCurrentBoundingSphereRadius);
    
    if (byteOk)
    {
        ParticleGPUInstancec newParticle;
        newParticle.force = modelNormal;
        newParticle.position = worldPosition.xyz;
        newParticle.speed = INIT_GPU_PARTICLE_SPEED;
        newParticle.time = 0.f;
        newParticle.color = input.modelColor;
        
        uint index;
        uint offset = outputRangeBuffer[1];
        if (offset > 0)
        {
            uint origValue;
            index = offset - 1;
            InterlockedExchange(outputRangeBuffer[1], offset - 1, origValue);
        }
        else
        {
            uint number = outputRangeBuffer[0];
            InterlockedExchange(outputRangeBuffer[0], number + 1, index);
        }
        
        outputParticleBuffer[index] = newParticle;
        
    }
    
    VS_OUTPUT output;
	{
        output.vWorldPosition = worldPosition;
        output.vPosition = clipPosition;
        output.vNormal = modelNormal;
        output.vColor = input.modelColor;
        output.vBitangent = mul(modelToWorld, float4(input.inBitangent, 0.0f));
        output.vTangent = mul(modelToWorld, float4(input.inTangent, 0.0f));
        output.vUV = input.inUV;
        output.vTime = input.modelTime;
    }
    return output;
}
