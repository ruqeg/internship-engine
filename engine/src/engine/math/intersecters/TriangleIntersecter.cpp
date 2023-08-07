#include <engine/math/intersecters/TriangleIntersecter.h>

namespace math
{

bool TriangleIntersecter::intersect(const Ray& ray, const Triangle& triangle, IntersectionRecord& rec)
{
	//Fast minimal storage ray triangle intersection
	//algorithm details: ()[https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf]
	const glm::vec3 e1 = triangle.p1 - triangle.p0;
	const glm::vec3 e2 = triangle.p2 - triangle.p0;
	const glm::vec3 p = glm::cross(ray.direction, e2);

	const float32 det = glm::dot(p, e1);
	if (det > -epsilon && det < epsilon) return false;
	const float32 invDet = 1.f / det;

	const glm::vec3 t = ray.origin - triangle.p0;
	const float32 u = glm::dot(p, t) * invDet;
	if (u < 0.f || u > 1.f) return false;

	const glm::vec3 q = glm::cross(t, e1);
	const float32 v = glm::dot(q, ray.direction) * invDet;
	if (v < 0.f || u + v > 1.f) return false;

	const float32 root = glm::dot(q, e2) * invDet;

	rec.t = root;
	rec.position = ray.origin + rec.t * ray.direction;
	if (rec.t > 0.f)
	{
		rec.normal = glm::normalize(glm::cross(triangle.p1 - triangle.p0, triangle.p2 - triangle.p0));
		return true;
	}
	return false;
}

}