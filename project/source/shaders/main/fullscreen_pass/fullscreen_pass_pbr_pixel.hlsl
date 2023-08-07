#include <engine/math_constants.hlsli>
#include <engine/algorithms/octahedron.hlsli>
#include "../../plasma_constant_buffer.hlsli"
#include "../../camera_constant_buffer.hlsli"
#include "../../material_constant_buffer.hlsli"
#include "../ibl_helper.hlsli"
#include "../shadow.hlsli"
#include "../lights_pbr.hlsli"

struct VS_OUTPUT
{
    float4 position : SV_Position;
    float2 screenUV : SCREENUV;
};

struct PS_OUTPUT
{
    float4 color : SV_Target;
    float depth : SV_Depth;
};

struct ParticleGPUInstancec
{
    float3 position;
    float3 force;
    float3 color;
    float speed;
    float time;
};

#define ALBEDO_TEXTURE_INDEX t0
#define ROUGHNESS_METALLNESS_TEXTURE_INDEX t1
#define TEXTURE_NORMAL_GEOM_NORMAL_TEXTURE_INDEX t2
#define EMISSION_TEXTURE_INDEX t3
#define DEPTH_TEXTURE_INDEX t4
#define OBJECID_TEXTURE_INDEX t5
#define GPU_PARTICLE_BUFFER_INDEX t7
#define RANGE_BUFFER_INDEX t8

Texture2D g_albedoTexture : register(ALBEDO_TEXTURE_INDEX);
Texture2D g_roughnessMetallnessTexture : register(ROUGHNESS_METALLNESS_TEXTURE_INDEX);
Texture2D g_texNormGeomNormTexture : register(TEXTURE_NORMAL_GEOM_NORMAL_TEXTURE_INDEX);
Texture2D g_emissionTexture : register(EMISSION_TEXTURE_INDEX);
Texture2D g_depthTexture : register(DEPTH_TEXTURE_INDEX);

StructuredBuffer<ParticleGPUInstancec> g_inputParticleBuffer : register(GPU_PARTICLE_BUFFER_INDEX);
Buffer<uint> g_inputRangeBuffer : register(RANGE_BUFFER_INDEX);


float3 VSPositionFromDepth(float2 vTexCoord, float depth);

PS_OUTPUT 
main
(VS_OUTPUT input)
{
    const float3 a = g_albedoTexture.Sample(g_sampler, input.screenUV);
    const float2 roughnessMetallness = g_roughnessMetallnessTexture.Sample(g_sampler, input.screenUV);
    const float4 texNormGeomNorm = g_texNormGeomNormTexture.Sample(g_sampler, input.screenUV);
    const float4 emission = g_emissionTexture.Sample(g_sampler, input.screenUV);
    const float depth = g_depthTexture.Sample(g_sampler, input.screenUV).r;
    
    const float r = roughnessMetallness.y;
    const float r2 = r * r;
    const float m = roughnessMetallness.x;
    const float3 n = unpackOctahedron(texNormGeomNorm.xy);
    const float3 gn = unpackOctahedron(texNormGeomNorm.zw);
    
    const float3 viewPosition = VSPositionFromDepth(input.screenUV, depth);
    const float3 p = mul(g_viewToWorld, float4(viewPosition, 1.f));
    const float3 v = normalize(g_cameraPosition - p);
    
    const float3 f0 = lerp(float3(0.04f, 0.04f, 0.04f), a, m);
    float3 E_direct = float3(0.0f, 0.0f, 0.0f);
    {
        for (int i = 0; i < g_pointLighsSize; ++i)
        {
            const float l = normalize(p - g_pointLights[i].position);
            const float shadow = pointlightShadowCalculation(p, i, dot(gn, l));
            E_direct += (1.f - shadow) * pointlightPerLightRadiance(g_pointLights[i], v, p, n, a, m, r2, f0);
        }
        for (int i = 0; i < g_spotLighsSize; ++i)
        {
            const float shadow = spotlightShadowCalculation(p, i, dot(gn, g_spotLights[i].direction));
            E_direct += (1.f - shadow) * spotlightPerLightRadiance(g_spotLights[i], v, p, n, a, m, r2, f0, g_flashlightMask);
        }
        for (int i = 0; i < g_directionalLighsSize; ++i)
        {
            const float shadow = directionallightShadowCalculation(p, i, dot(gn, g_directionalLights[i].direction));
            E_direct += (1.f - shadow) * directionallightPerLightRadiance(g_directionalLights[i], v, p, n, a, m, r2, f0);
        }
    }
    
    const uint particlesNum = g_inputRangeBuffer[0];
    const uint particlesOffset = g_inputRangeBuffer[1];
    
    for (int i = 0; i < particlesNum; ++i)
    {
        uint particleIndex = i + particlesOffset;
        ParticleGPUInstancec particle = g_inputParticleBuffer[particleIndex];
               
        const float3 lp = particle.position;
        const float3 l = normalize(lp - p);
        const float w = 0.1f;
        E_direct += renderingEquationWithCookTorranceBRDFOnlyDiff(particle.color, w, a, m, r2, f0, n, l, v, true);
    }
    
    const float3 Ediff = E_diffuseIrradiance(n);
    const float3 Espec = E_specularIrradiance(n, v, r);
    const float2 K_rg = E_specularReflectance(n, v, r);
    
    float3 Lo = float3(0.f, 0.f, 0.f);
    
    if (g_E_direct)
        Lo += E_direct;
    if (g_E_diff)
        Lo += Ediff * a * (1.f - m);
    if (g_E_spec)
        Lo += Espec * (f0 * K_rg.x + K_rg.y);
    
    PS_OUTPUT output;
    {
        output.color = float4(Lo, 1.f) + emission;
        output.depth = depth;
    }
    return output;
}

float3 VSPositionFromDepth(float2 vTexCoord, float depth)
{
    // Get the depth value for this pixel
    float z = depth;
    // Get x/w and y/w from the viewport position
    float x = vTexCoord.x * 2 - 1;
    float y = (1 - vTexCoord.y) * 2 - 1;
    float4 vProjectedPos = float4(x, y, z, 1.0f);
    // Transform by the inverse projection matrix
    float4 vPositionVS = mul(g_clipToView, vProjectedPos);
    // Divide by w to get the view-space position
    return vPositionVS.xyz / vPositionVS.w;
}