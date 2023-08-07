#ifndef __FRESNEL_LAW_HLSLI__
#define __FRESNEL_LAW_HLSLI__

/*
 * fd - lambertian diffuse model
 * fs - Cook-Torrance specular model
 * D - chosen NDF - normal distribution function - GGX
 * G - chosen geometry masking-shadowing function corresponding to D - height-correlated GGX Smith
 * F - law of Fresnel
 * 1/(4*dot(n,l)*dot(n,v)) - Cook-Torrance normalization factor
 * 1/pi - Lambertian normalization factor
 * fd has F(n), while fs has F(h) - technically everywhere should be F(h), because light reflects off micronormals h, but we use n for diffuse because complex diffuse reflection actually must not change its intensity for variable direction v
 * a - albedo
 * F0 - minimal specular reflection ratio when w=l
 * m - metalness
 * n - normal direction
 * v - view direction
 * l - light direction
 * h - halfvector of v and l
 * w - direction specified differently in diffuse and specular terms, see fd and f
 * r - squared roughness: when artists control a roughness slider and change value from 0 to 1, non-squared r put directly in formulas changes how perceived surface roughness looks in non-linear way. The value set by artists raised into the power two makes it easier to control. 
 */


#include "../../math_constants.hlsli"


float3 F(float cosTheta, float3 f0)
{
    return f0 + (1.0f - f0) * pow(clamp(1.0f - cosTheta, 0.0f, 1.0f), 5.0f);
}

#endif // __FRESNEL_LAW_HLSLI__