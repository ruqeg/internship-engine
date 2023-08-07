#ifndef COOK_TORRANCE_BDRF_HLSLI
#define COOK_TORRANCE_BDRF_HLSLI

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
 * l - light dir
 * w - direction specified differently in diffuse and specular terms, see fd and f
rection
 * h - halfvector of v and l
 * r - squared roughness: when artists control a roughness slider and change value from 0 to 1, non-squared r put directly in formulas changes how perceived surface roughness looks in non-linear way. The value set by artists raised into the power two makes it easier to control. 
 */


#include "../math_constants.hlsli"


float D(float r2, float ndoth);
float G(float r2, float ndotv, float ndotl);
float3 F(float cosTheta, float3 f0);


float pdot(const float3 v1, const float3 v2)
    { return max(dot(v1, v2), 0.0f); }


float3 cookTorranceBRDF(float3 a, float m, float r, float3 f0, float3 n, float3 l, float3 v)
{
    const float3 h = normalize(l + v);
    const float r2 = r * r;
    const float ndotl = pdot(n, l);
    const float hdotl = pdot(h, l);
    const float ndotv = pdot(n, v);
    const float ndoth = pdot(n, h);
    
    float3 fd;
    {
        const float3 fdNum = a * (1.0f - m) * (1.0 - F(ndotl, f0));
        fd = fdNum / PI;
    }
    float3 fs;
    {
        const float3 fsNum = D(r2, ndoth) * G(r2, ndotv, ndotl) * F(hdotl, f0);
        float3 invFsDenom;
        {
            const float3 invNdotL = max(1.0f / ndotl, 0.0f);
            const float3 invNdotV = max(1.0f / ndotv, 0.0f);
            invFsDenom = 0.25f * invNdotL * invNdotV;
        }
        fs = fsNum * invFsDenom;
    }
    return fd + fs;
}

#endif /* COOK_TORRANCE_BDRF_HLSLI */