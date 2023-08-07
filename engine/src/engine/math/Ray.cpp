#include <engine/math/Ray.h>

namespace math
{

Ray::Ray(const glm::vec3& origin, const glm::vec3& direction)
	: origin(origin), direction(direction)
{}

glm::vec3 Ray::at(float32 t) const
{
	return origin + direction * t;
}

}