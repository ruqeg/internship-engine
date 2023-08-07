#pragma once

#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
#include <engine/render/hittables/Hittable.h>

#include <vector>
#include <memory>

namespace render
{

struct LightRecord
{
	glm::vec3 direction;
	glm::vec3 color;
	glm::vec3 intensity;
	math::Ray unnormalVertexToLightRay;
};

struct LightSource
{
public:
	virtual bool illuminate(const glm::vec3& vertex, LightRecord& lightRec) const = 0;
	virtual bool shadowBetweenVertexAndLight(float32 t) const = 0;
protected:
	static constexpr float32 minShadowDistance = 0.f;
};

}