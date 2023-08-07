#include <engine/math/intersecters/MeshLocalIntersecter.h>

namespace math
{

bool MeshLocalIntersecter::intersect(const Ray& ray, const MeshLocal& meshLocal, IntersectionRecord& rec)
{
	return meshLocal.triangleOctree.intersect(ray, rec);
}

}