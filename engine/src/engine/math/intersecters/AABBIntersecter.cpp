#include <engine/math/intersecters/AABBIntersecter.h>

namespace math
{

bool AABBIntersecter::intersect(const Ray& ray, const AABB& aabb, float32& tmin, float32& tmax)
{
	//Ray-AABB (Axis Aligned Bounding Box) intersection 
	//algorithm details: (more fast)[https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection.html?source=post_page]
	const glm::vec3 invDir = 1.f / ray.direction;
	const glm::vec3 vtmin = (aabb.min - ray.origin) * invDir;
	const glm::vec3 vtmax = (aabb.max - ray.origin) * invDir;
	const glm::vec3 vt1 = glm::min(vtmin, vtmax);
	const glm::vec3 vt2 = glm::max(vtmin, vtmax);
	tmin = std::fmaxf(std::fmaxf(vt1.x, vt1.y), vt1.z);
	tmax = std::fminf(std::fminf(vt2.x, vt2.y), vt2.z);
	return tmin < tmax;
}

}