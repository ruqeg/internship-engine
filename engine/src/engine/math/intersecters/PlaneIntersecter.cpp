#include <engine/math/intersecters/PlaneIntersecter.h>

namespace math
{

bool PlaneIntersecter::intersect(const Ray& ray, const Plane& plane, IntersectionRecord& rec)
{
	//Ray-Plane intersection
	const float32 m = glm::dot(plane.normal, ray.direction);
	if (std::fabsf(m) < epsilon) return false;

	rec.t = -(plane.distance + glm::dot(plane.normal, ray.origin)) / m;
	rec.position = ray.origin + rec.t * ray.direction;
	rec.normal = plane.normal;
	return rec.t > 0.f;
}

}