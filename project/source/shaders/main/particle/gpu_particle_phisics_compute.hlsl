#include <engine/algorithms/octahedron.hlsli>
#include "../../global_samplers_states.hlsli"
#include "../../camera_constant_buffer.hlsli"

struct ParticleGPUInstancec
{
    float3 position;
    float3 force;
    float3 color;
    float speed;
    float time;
};

cbuffer ElapsedCB : register(b0)
{
    float g_elapsed;
    float3 _g_elapsed_cb_padding;
};

Texture2D g_texNormGeomNormTexture : register(t1);
Texture2D g_depthTexture : register(t2);

RWStructuredBuffer<ParticleGPUInstancec> g_outputParticleBuffer : register(u0);
RWBuffer<uint> g_outputRangeBuffer : register(u1);

static const float PARTICCLE_LIFETIME = 5.f;
static const float G = 6.0f;

#define THREADS_NUM 64

[numthreads(THREADS_NUM, 1, 1)]
void main(uint3 globalThreadId : SV_DispatchThreadID)
{
    uint number = g_outputRangeBuffer[0];
    uint offset = g_outputRangeBuffer[1];
    uint perThread = number / THREADS_NUM;
    uint currentPerThread = perThread;
    
    //for last thread
    if (globalThreadId.x == (THREADS_NUM - 1))
        currentPerThread = perThread + number % THREADS_NUM;
    
    for (uint i = 0; i < currentPerThread; ++i)
    {
        const uint bufferIndex = offset + globalThreadId.x * perThread + i;
        
        ParticleGPUInstancec particle = g_outputParticleBuffer[bufferIndex];
        
        particle.time += g_elapsed;
        if (particle.time > PARTICCLE_LIFETIME)
        {
            g_outputRangeBuffer[2] = g_outputRangeBuffer[2] + 1;
            continue;
        }
        
        float3 newForce = particle.force + float3(0.f, -G, 0.f) * g_elapsed;
        float3 newPosition = particle.position + particle.force * g_elapsed;
        
        const float4 viewPosition = mul(g_worldToView, float4(newPosition, 1.f));
        const float4 clipPosition = mul(g_viewToClip, viewPosition);
        
        const float4 uClipPosition = clipPosition / clipPosition.w;
        
        if (abs(uClipPosition.x) < 1.f || abs(uClipPosition.y) < 1.f)
        {
            const float2 screenUV = float2(
                0.5f * uClipPosition.x + 0.5f,
                -0.5f * uClipPosition.y + 0.5f);
        
            const float particleDepth = uClipPosition.z;
            const float sceneDepth = g_depthTexture.SampleLevel(g_sampler, screenUV, 0u).r;
            const float depthDiff = 1.f / sceneDepth - 1.f / particleDepth;
        
            const float3 normal = unpackOctahedron(g_texNormGeomNormTexture.SampleLevel(g_sampler, screenUV, 0u).zw);
        
            if (depthDiff < 0.01f)
            {
                newForce = reflect(particle.force, normal) * particle.speed;
                newPosition = particle.position + newForce * g_elapsed;
                particle.speed *= 0.85f;
            }
        }
        
        particle.force = newForce;
        particle.position = newPosition;
        
        g_outputParticleBuffer[bufferIndex] = particle;
    }
}