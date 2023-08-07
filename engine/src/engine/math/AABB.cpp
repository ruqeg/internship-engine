#include <engine/math/AABB.h>

namespace math
{

AABB::AABB(const glm::vec3& min, const glm::vec3& max)
	: min(min), max(max)
{}

bool AABB::contains(const glm::vec3& point) const
{
	return
		point.x >= min.x && point.x <= max.x &&
		point.y >= min.y && point.y <= max.y &&
		point.z >= min.z && point.z <= max.z;
}

}