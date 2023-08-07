#ifndef SPOTLIGHT_HLSLI
#define SPOTLIGHT_HLSLI

#include "../light_constant_buffer.hlsli"
#include "../global_samplers_states.hlsli"
#include "cbuffer_declaration.hlsli"

float3 spotLightIntensityMask(const SpotLightsCBS spotlight, const float3 vertex, Texture2D mask, const float3 colorOutside = float3(0.f, 0.f, 0.f), const float3 up = float3(0.f, 1.f, 0.f))
{
    const float CONSTANT_TAN_ANGLE = tan(spotlight.angle);
    const float CONSTANT_COS_ANGLE = cos(spotlight.angle);
  
    const float3 vp = vertex - spotlight.position;
    const float distance = length(vp);
    const float3 nvp = vp / distance;

    //TODO convert to matrix
    const float vpPorjectionLength = dot(vp, spotlight.direction);
    const float diskRadius = CONSTANT_TAN_ANGLE * vpPorjectionLength;
    
    const float3 xaxis = cross(up, spotlight.direction);
    const float3 nxaxis = normalize(xaxis);

    const float3 yaxis = cross(spotlight.direction, xaxis);
    const float3 nyaxis = normalize(yaxis);
    
    const float u = dot(nxaxis, vp) / (2 * diskRadius) + 0.5f;
    const float v = dot(nyaxis, vp) / (2 * diskRadius) + 0.5f;
	
    if (u < 0 || v < 0 || u > 1 || v > 1)
        return colorOutside;
    
    const float3 flashlightMaskTexel = mask.Sample(g_sampler, float2(u, v));
		
    return flashlightMaskTexel * spotlight.intensity;
}

#endif //SPOTLIGHT_HLSLI