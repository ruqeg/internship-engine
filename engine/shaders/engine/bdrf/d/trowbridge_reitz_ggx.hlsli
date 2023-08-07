#ifndef __TROWBRIDGE_REITZ_GGX_HLSLI__
#define __TROWBRIDGE_REITZ_GGX_HLSLI__

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


float D(float r2, float ndoth)
{
    const float num = r2;
    float denom;
    {
        const float y = ndoth * ndoth * (r2 - 1.0f) + 1.0f;
        denom = y * y * PI;
    }
    return num / denom;

}

#endif // __TROWBRIDGE_REITZ_GGX_HLSLI__