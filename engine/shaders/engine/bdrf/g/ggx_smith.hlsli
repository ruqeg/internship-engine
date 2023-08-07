#ifndef __GGX_SMITH_HLSLI__
#define __GGX_SMITH_HLSLI__

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


float G(float r4, float ndotv, float ndotl)
{
    const float ndotv2 = ndotv * ndotv;
    const float ndotl2 = ndotl * ndotl;
    const float num = 2.0f;
    const float denom = sqrt(1.0f + r4 * (1.0f - ndotv2) / ndotv2) + sqrt(1 + r4 * (1.0f - ndotl2) / ndotl2);
    return num / denom;
}

#endif // __GGX_SMITH_HLSLI__