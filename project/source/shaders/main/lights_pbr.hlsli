#include <engine/bdrf/rendering_equation.hlsli>
#include <engine/bdrf/d/trowbridge_reitz_ggx.hlsli>
#include <engine/bdrf/g/ggx_smith.hlsli>
#include <engine/bdrf/f/fresnel_law.hlsli>
#include "spotlight.hlsli"

void getPointLightIntesivity(float3 l, float3 v, float3 n, float distL, float r2, float sphereRadius, float lightRadius, out float falloff);
float calculateSolidAngle(float3 lightPosition, float lightRadius2, float3 vertex, out float lightDiametr);
float calculateSolidAngle(float3 lightPosition, float lightRadius2, float3 vertex);

float3 pointlightPerLightRadiance(PointLightsCBS pointlight, const float3 v, const float3 p, const float3 n, const float3 a, const float m, const float r2, const float3 f0)
{
    const float3 lp = pointlight.position;
    const float lr = pointlight.radius;
    const float lr2 = lr * lr;
    
    const float3 l = normalize(lp - p);
    const float3 h = normalize(v + l);
    float ld;
    const float w = calculateSolidAngle(lp, lr2, p, ld);
    float falloff1, falloff2;
    getPointLightIntesivity(l, v, n, ld, r2, lr, lr2, falloff1);
    getPointLightIntesivity(l, v, n, ld, r2, lr, lr2, falloff2);
    return renderingEquationWithCookTorranceBRDF(pointlight.intensity, w, a, m, r2, f0, n, l, v, true) * falloff1 * falloff2;
}

float3 pointlightPerLightRadianceOnlyDiff(PointLightsCBS pointlight, const float3 v, const float3 p, const float3 n, const float3 a, const float m, const float r2, const float3 f0)
{
    const float3 lp = pointlight.position;
    const float lr = pointlight.radius;
    const float lr2 = lr * lr;
    
    const float3 l = normalize(lp - p);
    const float3 h = normalize(v + l);
    float ld;
    const float w = calculateSolidAngle(lp, lr2, p, ld);
    float falloff1, falloff2;
    getPointLightIntesivity(l, v, n, ld, r2, lr, lr2, falloff1);
    getPointLightIntesivity(l, v, n, ld, r2, lr, lr2, falloff2);
    return renderingEquationWithCookTorranceBRDFOnlyDiff(pointlight.intensity, w, a, m, r2, f0, n, l, v, true) * falloff1 * falloff2;
}

float3 spotlightPerLightRadiance(SpotLightsCBS spotlight, const float3 v, const float3 p, const float3 n, const float3 a, const float m, const float r2, const float3 f0, Texture2D mask)
{
    const float3 lp = spotlight.position;
    const float3 l = normalize(lp - p);
    const float3 h = normalize(v + l);
    float ld;
    const float w = calculateSolidAngle(lp, spotlight.radius, p, ld);
    return renderingEquationWithCookTorranceBRDF(spotLightIntensityMask(spotlight, p, mask), w, a, m, r2, f0, n, l, v, false);
}

float3 directionallightPerLightRadiance(DirectionalLightsCBS directionallight, const float3 v, const float3 p, const float3 n, const float3 a, const float m, const float r2, const float3 f0)
{
    const float3 l = -directionallight.direction;
    const float3 h = normalize(v + l);
    const float w = directionallight.solidAngle;
    return renderingEquationWithCookTorranceBRDF(directionallight.intensity, w, a, m, r2, f0, n, l, v, false);
}

void getPointLightIntesivity(float3 l, float3 v, float3 n, float distL, float r2, float sphereRadius, float lightRadius, out float falloff)
{
    float3 r = reflect(-v, n);
    float3 centerToRay = (dot(l, r) * r) - l;
    float3 closestPoint = l + centerToRay * saturate(sphereRadius / length(centerToRay));
    l = normalize(closestPoint);
    float distLight = length(closestPoint);
    float alphaPrime = saturate(sphereRadius / (distL * 2.0) + r2);
    falloff = pow(saturate(1.0 - pow(distLight / (lightRadius), 4)), 2) / ((distLight * distLight) + 1.0);
}

float calculateSolidAngle(float3 lightPosition, float lightRadius2, float3 vertex, out float lightDiametr)
{
    lightDiametr = length(lightPosition - vertex);
    const float lightDiametr2 = lightDiametr * lightDiametr;
    const float w = 1.0f - sqrt(max(1.0f - lightRadius2 / lightDiametr2, 0.0f));
    return w;
}

float calculateSolidAngle(float3 lightPosition, float lightRadius2, float3 vertex)
{
    float lightDiametr;
    return calculateSolidAngle(lightPosition, lightRadius2, vertex, lightDiametr);
}