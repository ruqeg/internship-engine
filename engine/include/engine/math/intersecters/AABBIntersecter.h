#pragma once

#include <engine/types.h>

#include <engine/math/AABB.h>
#include <engine/math/Ray.h>

#include <glm/gtx/extended_min_max.hpp>

namespace math
{

class AABBIntersecter
{
public:
	static bool intersect(const Ray& ray, const AABB& aabb, float32& tmin, float32& tmax);
};

}