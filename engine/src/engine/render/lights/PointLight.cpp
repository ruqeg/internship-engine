#include <engine/render/lights/PointLight.h>

namespace render
{

bool PointLight::illuminate(const glm::vec3& vertex, LightRecord& lightRec) const
{
	const glm::vec3 vp = vertex - position;
	const float32 distance = glm::length(vp);
	lightRec.direction = vp / distance;
	lightRec.intensity = std::powf(1.f - distance / radius, scatter) * intensity;
	lightRec.color = color;
	lightRec.unnormalVertexToLightRay.origin = vertex;
	lightRec.unnormalVertexToLightRay.direction = position - vertex;

	return distance <= radius;
}

bool PointLight::shadowBetweenVertexAndLight(float32 t) const
{
	return t < 1.f && t > minShadowDistance;
}

}