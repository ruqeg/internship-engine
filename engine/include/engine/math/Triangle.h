#pragma once

#include <glm/vec3.hpp>

namespace math
{

struct Triangle
{
public:
	Triangle() = default;
	Triangle(const glm::vec3& p0, const glm::vec3& p1, const glm::vec3& p2) : p0(p0), p1(p1), p2(p2) {}
public:
	glm::vec3 p0;
	glm::vec3 p1;
	glm::vec3 p2;
};

}