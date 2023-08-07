#pragma once

#include <engine/math/intersecters/Intersecter.h>

#include <engine/math/Triangle.h>

#include <glm/geometric.hpp>

namespace math
{

class TriangleIntersecter : public Intersecter
{
public:
	static bool intersect(const Ray& ray, const Triangle& trianlge, IntersectionRecord& rec);
};

}