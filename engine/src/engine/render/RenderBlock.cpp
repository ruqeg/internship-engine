#include <engine/render/RenderBlock.h>

namespace render
{

RenderBlock::RenderBlock(const math::Camera* camera, const math::Frustum* cameraFrustum, const render::World* world)
	: camera(camera), cameraFrustum(cameraFrustum), world(world)
{}

void RenderBlock::operator()(const UnsafeOptBuffer& buffer,	int32_t startInd, int32_t endInd) const
{
	for (int64 i = startInd; i < endInd; ++i)
	{
		const int32 x = i % buffer.width;
		const int32 y = i / buffer.width;
		const float32 u = x * buffer.invwidth;
		const float32 v = y * buffer.invheight;

		const glm::vec3 horizontal = cameraFrustum->getNearHorizontal();
		const glm::vec3 vertical = cameraFrustum->getNearVertical();
		const glm::vec3 leftTopCorner = cameraFrustum->getNearLeftTop();
		const glm::vec3 currentPixelNearClipPosition = leftTopCorner + u * horizontal - v * vertical;
		const math::Ray viewRay(camera->getPosition(), currentPixelNearClipPosition - camera->getPosition());

		glm::vec3 currentColor;
		HitRecord nearestHitRec;
		std::shared_ptr<Hittable> nearestHittable;
		if (world->hit(viewRay, nearestHittable, nearestHitRec))
		{
			currentColor = BlinnPhongHitColor(-1.f * glm::normalize(viewRay.direction), nearestHittable, nearestHitRec);
		}
		else
		{
			currentColor = world->backgroundColor;
		}

		buffer.data[i] = utils::ColorConverter::colorToPixel(currentColor);
	}
}

glm::vec3 RenderBlock::BlinnPhongHitColor(const glm::vec3& normInViewRayDir, const std::shared_ptr<Hittable>& shadowIgnoredHittable, const HitRecord& hitRec) const
{
	glm::vec3 ambientColor(0.f, 0.f, 0.f);
	glm::vec3 diffuseColor(0.f, 0.f, 0.f);
	glm::vec3 specularColor(0.f, 0.f, 0.f);

	for (const auto& lightSource : world->lightsSources)
	{
		LightRecord lightRec;
		if (lightSource->illuminate(hitRec.position, lightRec))
		{
			if (VertexLightInShadow(hitRec.position, shadowIgnoredHittable, lightSource, lightRec))
			{
				continue;
			}

			const glm::vec3 vertLightColor = lightRec.color * lightRec.intensity;

			// ambient
			ambientColor += vertLightColor;

			// diffuse
			const float32 diff = std::fmax(glm::dot(hitRec.normal, -lightRec.direction), minLightSourceVisibility);
			diffuseColor += vertLightColor * diff;

			// specular
			const glm::vec3 halfViewLight = glm::normalize(normInViewRayDir - lightRec.direction);
			const float32 dotr = std::fmax(glm::dot(hitRec.normal, halfViewLight), minLightSourceVisibility);
			const float32 spec = std::powf(dotr, hitRec.material->shininess);
			specularColor += vertLightColor * spec;
		}
	}

	return (ambientColor * hitRec.material->ambient) + (diffuseColor * hitRec.material->diffuse) + (specularColor * hitRec.material->specular);
}

bool RenderBlock::VertexLightInShadow(
	const glm::vec3& vertex, const std::shared_ptr<Hittable>& shadowIgnoredHittable,
	const std::shared_ptr<LightSource>& lightSource, const LightRecord& lightRec) const
{
	for (const auto& hittable : world->shadingHittables)
	{
		if (hittable == shadowIgnoredHittable)
		{
			continue;
		}

		HitRecord shadowHitRec;
		if (hittable->hit(lightRec.unnormalVertexToLightRay, shadowHitRec))
		{
			if (lightSource->shadowBetweenVertexAndLight(shadowHitRec.t))
			{
				return true;
			}
		}
	}
	return false;
}

}