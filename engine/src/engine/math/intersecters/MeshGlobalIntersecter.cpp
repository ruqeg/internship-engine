#include <engine/math/intersecters/MeshGlobalIntersecter.h>

namespace math
{

bool MeshGlobalIntersecter::intersect(const Ray& ray, const MeshGlobal& meshGlobal, IntersectionRecord& rec)
{
	math::Ray localRay;
	{
		const glm::vec3 od = ray.origin + ray.direction;
		const glm::vec3 localRayEnd = meshGlobal.toMatInv() * glm::vec4(od, 1.f);
		localRay.origin = meshGlobal.toMatInv() * glm::vec4(ray.origin, 1.f);
		localRay.direction = localRayEnd - localRay.origin;
	}

	IntersectionRecord localIntersectionRec;
	if (MeshLocalIntersecter::intersect(localRay, *meshGlobal.getMeshLocal(), localIntersectionRec))
	{
		rec.position = meshGlobal.toMat() * glm::vec4(localIntersectionRec.position, 1.f);
		rec.t = localIntersectionRec.t;
		rec.normal = meshGlobal.getRotationMat() * glm::vec4(localIntersectionRec.normal, 1.f);
		return true;
	}

	return false;
}

}