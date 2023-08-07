#pragma once

#include <engine/math/intersecters/RIntersecter.h>
#include <engine/math/RTriangle.h>

namespace math
{

class RTriangleIntersecter : public Intersecter
{
public:
	static bool intersect(const RRay& ray, const RTriangle& trianlge, RIntersectionRecord& rec);
};

}