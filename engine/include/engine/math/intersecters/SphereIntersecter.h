#pragma once

#include <engine/math/intersecters/Intersecter.h>

#include <engine/math/Sphere.h>

#include <glm/geometric.hpp>

namespace math
{

class SphereIntersecter : public Intersecter
{
public:
	static bool intersect(const Ray& ray, const Sphere& sphere, IntersectionRecord& rec);
};

}