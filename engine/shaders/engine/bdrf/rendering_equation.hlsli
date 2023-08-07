#ifndef __RENDERING_EQUATION_HLSLI__
#define __RENDERING_EQUATION_HLSLI__

#include "cook_torrance_bdrf.hlsli"

void SphereMaxNoH(float NoV, inout float NoL, inout float VoL, float sphereSin, float sphereCos, bool bNewtonIteration, out float NoH, out float VoH)
{
    float RoL = 2 * NoL * NoV - VoL;
    if (RoL >= sphereCos)
    {
        NoH = 1;
        VoH = abs(NoV);
    }
    else
    {
        float rInvLengthT = sphereSin * rsqrt(1 - RoL * RoL);
        float NoTr = rInvLengthT * (NoV - RoL * NoL);
        float VoTr = rInvLengthT * (2 * NoV * NoV - 1 - RoL * VoL);

        if (bNewtonIteration && sphereSin != 0)
        {
			// dot( cross(N,L), V )
            float NxLoV = sqrt(saturate(1 - pow(NoL, 2) - pow(NoV, 2) - pow(VoL, 2) + 2 * NoL * NoV * VoL));

            float NoBr = rInvLengthT * NxLoV;
            float VoBr = rInvLengthT * NxLoV * 2 * NoV;

            float NoLVTr = NoL * sphereCos + NoV + NoTr;
            float VoLVTr = VoL * sphereCos + 1 + VoTr;

            float p = NoBr * VoLVTr;
            float q = NoLVTr * VoLVTr;
            float s = VoBr * NoLVTr;

            float xNum = q * (-0.5 * p + 0.25 * VoBr * NoLVTr);
            float xDenom = p * p + s * (s - 2 * p) + NoLVTr * ((NoL * sphereCos + NoV) * pow(VoLVTr, 2) + q * (-0.5 * (VoLVTr + VoL * sphereCos) - 0.5));
            float TwoX1 = 2 * xNum / (pow(xDenom, 2) + pow(xNum, 2));
            float SinTheta = TwoX1 * xDenom;
            float CosTheta = 1.0 - TwoX1 * xNum;
            NoTr = CosTheta * NoTr + SinTheta * NoBr;
            VoTr = CosTheta * VoTr + SinTheta * VoBr;
        }

        NoL = NoL * sphereCos + NoTr;
        VoL = VoL * sphereCos + VoTr;

        float InvLenH = rsqrt(2 + 2 * VoL);
        NoH = saturate((NoL + NoV) * InvLenH);
        VoH = saturate(InvLenH + InvLenH * VoL);
    }
}

float3 renderingEquationWithCookTorranceBRDF(float3 I, float w, float3 a, float m, float r, float3 f0, float3 n, float3 l, float3 v, bool isSphereMaxNoHOn)
{
    float3 h = normalize(l + v);
    float r2 = r * r;
    float hdotl = pdot(h, l);
    float ndotv = pdot(n, v);
    float ndoth = pdot(n, h);
    float vdoth = pdot(v, h);
    float vdotl = pdot(v, l);
    float ndotl = pdot(n, l);
    
    const float3 fdNum = w * a * (1.0f - m) * (1.0 - F(ndotl, f0)) * ndotl;
    const float3 fd = fdNum / PI;
    
    const float3 miny = min(1.0f, (D(r2, ndoth) * w) / (4.0f * ndotv));
    if (isSphereMaxNoHOn)
        SphereMaxNoH(ndotv, ndotl, vdotl, sin(w / 2), cos(w / 2), false, ndoth, vdoth);
    const float3 fs = miny * G(r2, ndotv, ndotl) * F(hdotl, f0);
    
    return I * (fd + fs);
}

float3 renderingEquationWithCookTorranceBRDFOnlyDiff(float3 I, float w, float3 a, float m, float r, float3 f0, float3 n, float3 l, float3 v, bool isSphereMaxNoHOn)
{
    float3 h = normalize(l + v);
    float r2 = r * r;
    float hdotl = pdot(h, l);
    float ndotv = pdot(n, v);
    float ndoth = pdot(n, h);
    float vdoth = pdot(v, h);
    float vdotl = pdot(v, l);
    float ndotl = pdot(n, l);
    
    const float3 fdNum = w * a * (1.0f - m) * (1.0 - F(ndotl, f0)) * ndotl;
    const float3 fd = fdNum / PI;
    
    return I * fd;
}

#endif //__RENDERING_EQUATION_HLSLI__