#pragma once

#include <glm/vec3.hpp>

namespace math
{

struct AABB
{
public:
	AABB() = default;
	AABB(const glm::vec3& min, const glm::vec3& max);
	bool contains(const glm::vec3& point) const;
public:
	glm::vec3 min;
	glm::vec3 max;
};

}