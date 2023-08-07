#pragma once

#include <engine/render/Material.h>
#include <engine/render/World.h>
#include <engine/math/Camera.h>
#include <engine/math/Rectangle.h>
#include <engine/math/Frustum.h>
#include <engine/utils/ColorConverter.h>

#include <vector>
#include <memory>

namespace render
{

struct UnsafeOptBuffer
{
	uint32_t* data;
	int32_t width;
	int32_t height;
	float invwidth;
	float invheight;
};

class RenderBlock
{
public:
	explicit RenderBlock(const math::Camera* camera, const math::Frustum* cameraFrustum, const render::World* world);
	void operator()(const UnsafeOptBuffer& buffer, int32_t startInd, int32_t endInd) const;
private:
	glm::vec3 BlinnPhongHitColor(const glm::vec3& normInViewRayDir, const std::shared_ptr<Hittable>& shadowIgnoredHittable,	const HitRecord& hitRec) const;
	bool VertexLightInShadow(
		const glm::vec3& vertex, const std::shared_ptr<Hittable>& shadowIgnoredHittable, 
		const std::shared_ptr<LightSource>& lightSource, const LightRecord& lightRec) const;
private:
	const math::Camera* camera;
	const math::Frustum* cameraFrustum;
	const render::World* world;
private:
	static constexpr float64 minLightSourceVisibility = 0.0001f;
};

}