#include <engine/math/intersecters/SphereIntersecter.h>

namespace math
{

bool SphereIntersecter::intersect(const Ray& ray, const Sphere& sphere, IntersectionRecord& rec)
{
	//Ray-Sphere intersection
	//algorithm details: (5 Adding a Sphere)[https://raytracing.github.io/books/RayTracingInOneWeekend.html]
	const glm::vec3 oc = ray.origin - sphere.center;
	const float32 a = glm::dot(ray.direction, ray.direction);
	const float32 b = 2.f * glm::dot(oc, ray.direction);
	const float32 c = glm::dot(oc, oc) - sphere.radius * sphere.radius;

	const float32 discriminant = b * b - 4 * a * c;
	if (discriminant < 0.f) return false;
	const float32 sqrtd = sqrt(discriminant);

	const float32 inv2a = 1.f / (2.f * a);
	const float32 root1 = (-b - sqrtd) * inv2a;
	const float32 root2 = (-b + sqrtd) * inv2a;

	rec.t = std::fminf(root1, root2);
	rec.position = ray.origin + rec.t * ray.direction;
	if (rec.t > 0.f)
	{
		rec.normal = glm::normalize(rec.position - sphere.center);
		return true;
	}
	return false;
}

}