#include <engine/math/intersecters/RAABBIntersecter.h>

using namespace DirectX;

namespace math
{

bool RAABBIntersecter::intersect(const RRay& ray, const RAABB& aabb, float32& tmin, float32& tmax)
{
	//Ray-AABB (Axis Aligned Bounding Box) intersection 
	//algorithm details: (more fast)[https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection.html?source=post_page]
	const XMVECTOR invDir = XMVectorReciprocal(ray.direction);
	const XMVECTOR vtmin = XMVectorMultiply(XMVectorSubtract(aabb.min, ray.origin), invDir);
	const XMVECTOR vtmax = XMVectorMultiply(XMVectorSubtract(aabb.max, ray.origin), invDir);
	const XMVECTOR vt1 = XMVectorMin(vtmin, vtmax);
	const XMVECTOR vt2 = XMVectorMax(vtmin, vtmax);
	tmin = std::fmaxf(std::fmaxf(XMVectorGetX(vt1), XMVectorGetY(vt1)), XMVectorGetZ(vt1));
	tmax = std::fminf(std::fminf(XMVectorGetX(vt2), XMVectorGetY(vt2)), XMVectorGetZ(vt2));
	return tmin < tmax;
}

}