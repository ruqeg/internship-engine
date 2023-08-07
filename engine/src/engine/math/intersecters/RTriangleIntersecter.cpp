#include <engine/math/intersecters/RTriangleIntersecter.h>

using namespace DirectX;

namespace math
{

bool RTriangleIntersecter::intersect(const RRay& ray, const RTriangle& triangle, RIntersectionRecord& rec)
{
	//Fast minimal storage ray triangle intersection
	//algorithm details: ()[https://cadxfem.org/inf/Fast%20MinimumStorage%20RayTriangle%20Intersection.pdf]
	const XMVECTOR e1 = XMVectorSubtract(triangle.p1, triangle.p0);
	const XMVECTOR e2 = XMVectorSubtract(triangle.p2, triangle.p0);
	const XMVECTOR p = XMVector3Cross(ray.direction, e2);

	const float32 det = XMVectorGetX(XMVector3Dot(p, e1));
	if (det > -epsilon && det < epsilon) return false;
	const float32 invDet = 1.f / det;

	const XMVECTOR t = ray.origin - triangle.p0;
	const float32 u = XMVectorGetX(XMVector3Dot(p, t)) * invDet;
	if (u < 0.f || u > 1.f) return false;

	const XMVECTOR q = XMVector3Cross(t, e1);
	const float32 v = XMVectorGetX(XMVector3Dot(q, ray.direction)) * invDet;
	if (v < 0.f || u + v > 1.f) return false;

	const float32 root = XMVectorGetX(XMVector3Dot(q, e2)) * invDet;

	rec.t = root;
	rec.position = XMVectorAdd(ray.origin, XMVectorScale(ray.direction, rec.t));
	if (rec.t > 0.f)
	{
		const XMVECTOR p10 = XMVectorSubtract(triangle.p1, triangle.p0);
		const XMVECTOR p20 = XMVectorSubtract(triangle.p2, triangle.p0);
		const XMVECTOR crossv = XMVector3Cross(p10, p20);
		rec.normal = XMVector3Normalize(crossv);
		return true;
	}
	return false;
}

}