#include <engine/render/lights/SunLight.h>

namespace render
{

bool SunLight::illuminate(const glm::vec3& vertex, LightRecord& lightRec) const
{
	lightRec.direction = direction;
	lightRec.intensity = intensity;
	lightRec.color = color;
	lightRec.unnormalVertexToLightRay.origin = vertex;
	lightRec.unnormalVertexToLightRay.direction = -direction;
	return true;
}

bool SunLight::shadowBetweenVertexAndLight(float32 t) const
{
	return t > minShadowDistance;
}

}