#pragma once

#include <engine/math/RAABB.h>
#include <engine/math/RRay.h>
#include <cmath>

namespace math
{

class RAABBIntersecter
{
public:
	static bool intersect(const RRay& ray, const RAABB& aabb, float32& tmin, float32& tmax);
};

}