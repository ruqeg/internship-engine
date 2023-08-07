#include <engine/render/lights/SpotLight.h>

namespace render
{

bool SpotLight::illuminate(const glm::vec3& vertex, LightRecord& lightRec) const
{
	const glm::vec3 vp = vertex - position;
	const glm::vec3 nvp = glm::normalize(vp);

	if (std::acosf(glm::dot(nvp, direction)) <= angle)
	{
		const float32 distance = glm::length(vp);
		if (distance <= length)
		{
			lightRec.direction = vp / distance;
			lightRec.intensity = intensity;
			lightRec.color = color;
			lightRec.unnormalVertexToLightRay.origin = vertex;
			lightRec.unnormalVertexToLightRay.direction = position - vertex;
			return true;
		}
	}
	return false;
}

bool SpotLight::shadowBetweenVertexAndLight(float32 t) const
{
	return t < 1.f && t > minShadowDistance;
}

}