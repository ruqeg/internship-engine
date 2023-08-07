#pragma once

#include <engine/math/intersecters/Intersecter.h>

#include <engine/math/Plane.h>

#include <glm/geometric.hpp>

namespace math
{

class PlaneIntersecter : public Intersecter
{
public:
	static bool intersect(const Ray& ray, const Plane& plane, IntersectionRecord& rec);
};

}