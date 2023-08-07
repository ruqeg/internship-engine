#pragma once

#include <engine/types.h>

#include <glm/vec3.hpp>

namespace math
{

struct Sphere
{
public:
	Sphere() = default;
	Sphere(const glm::vec3& center, float32 radius) : center(center), radius(radius) {}
public:
	glm::vec3 center;
	float32 radius;
};

}