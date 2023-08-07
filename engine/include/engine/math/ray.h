#pragma once

#include <engine/types.h>

#include <glm/vec3.hpp>

namespace math
{

struct Ray
{
public:
	Ray() = default;
	Ray(const glm::vec3& origin, const glm::vec3& direction);
	glm::vec3 at(float32 t) const;
public:
	glm::vec3 origin;
	glm::vec3 direction;
};

}