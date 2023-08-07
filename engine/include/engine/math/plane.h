#pragma once

#include <engine/types.h>

#include <glm/vec3.hpp>

namespace math
{

struct Plane
{
public:
	Plane() = default;
	Plane(const glm::vec3& normal, float32 distance) : normal(normal), distance(distance) {}
public:
	glm::vec3 normal;
	float32 distance;
};

}