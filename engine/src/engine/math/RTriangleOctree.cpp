#include <engine/math/RTriangleOctree.h>

using namespace DirectX;

namespace math
{

RTriangleOctree::RTriangleOctree(const RTriangleOctree& other)
	: region(other.region), triangles(other.triangles), childNode(other.childNode)
{}

RTriangleOctree& RTriangleOctree::operator=(const RTriangleOctree& other)
{
	region = other.region;
	triangles = other.triangles;
	childNode = other.childNode;
	return *this;
}

RTriangleOctree::RTriangleOctree(const RAABB& region)
	:
	region(RAABB(XMVectorSubtract(region.min, epsilon), XMVectorAdd(region.max, epsilon)))
{}

bool RTriangleOctree::build(const std::vector<RTriangle>& newTriangles, const RAABB& newRegion)
{
	triangles.clear();
	triangles.shrink_to_fit();

	childNode = nullptr;

	region = RAABB(XMVectorSubtract(newRegion.min, epsilon), XMVectorAdd(newRegion.max, epsilon));

	for (const auto& triangle : newTriangles)
	{
		const XMVECTOR tc = (triangle.p0 + triangle.p1 + triangle.p2) / 3.f;
		if (!addTriangle(triangle, tc))
		{
			return false;
		}
	}
	return true;
}

bool RTriangleOctree::addTriangle(const RTriangle& triangle, const XMVECTOR& c)
{
	if (!region.contains(c))
	{
		return false;
	}

	if (!region.contains(triangle.p0) || !region.contains(triangle.p1) || !region.contains(triangle.p2))
	{
		return false;
	}

	if (childNode == nullptr)
	{
		if (triangles.size() < prefferedTriangleCount)
		{
			triangles.emplace_back(triangle);
			return true;
		}
		else
		{
			const XMVECTOR mn = region.min;
			const XMVECTOR mx = region.max;
			const XMVECTOR c = (region.min + region.max) / 2.f;

			childNode.reset(new std::array<RTriangleOctree, 8>());
			(*childNode)[0] = RTriangleOctree(RAABB(mn, c));
			(*childNode)[1] = RTriangleOctree(RAABB({ XMVectorGetX(c), XMVectorGetY(mn), XMVectorGetZ(mn) }, { XMVectorGetX(mx), XMVectorGetY(c), XMVectorGetZ(c) }));
			(*childNode)[2] = RTriangleOctree(RAABB({ XMVectorGetX(c), XMVectorGetY(mn), XMVectorGetZ(c) }, { XMVectorGetX(mx), XMVectorGetY(c), XMVectorGetZ(mx) }));
			(*childNode)[3] = RTriangleOctree(RAABB({ XMVectorGetX(mn), XMVectorGetY(mn), XMVectorGetZ(c) }, { XMVectorGetX(c), XMVectorGetY(c), XMVectorGetZ(mx) }));
			(*childNode)[4] = RTriangleOctree(RAABB({ XMVectorGetX(mn), XMVectorGetY(c), XMVectorGetZ(mn) }, { XMVectorGetX(c), XMVectorGetY(mx), XMVectorGetZ(c) }));
			(*childNode)[5] = RTriangleOctree(RAABB({ XMVectorGetX(c), XMVectorGetY(c), XMVectorGetZ(mn) }, { XMVectorGetX(mx), XMVectorGetY(mx), XMVectorGetZ(c) }));
			(*childNode)[6] = RTriangleOctree(RAABB(c, mx));
			(*childNode)[7] = RTriangleOctree(RAABB({ XMVectorGetX(mn), XMVectorGetY(c), XMVectorGetZ(c) }, { XMVectorGetX(c), XMVectorGetY(mx), XMVectorGetZ(mx) }));
			std::vector<RTriangle> newTriangles;

			for (const auto& triangle : triangles)
			{
				if (!addTriangleToChildNode(triangle))
				{
					newTriangles.emplace_back(triangle);
				}
			}

			triangles = std::move(newTriangles);
		}
	}

	if (!addTriangleToChildNode(triangle))
	{
		triangles.emplace_back(triangle);
	}

	return true;
}

bool RTriangleOctree::addTriangleToChildNode(const RTriangle& triangle)
{
	const XMVECTOR triangleCenter = (triangle.p0 + triangle.p1 + triangle.p2) / 3.f;

	for (int32 i = 0; i < 8; ++i)
	{
		if ((*childNode)[i].addTriangle(triangle, triangleCenter))
		{
			return true;
		}
	}

	return false;
}

bool RTriangleOctree::intersect(const RRay& ray, RIntersectionRecord& rec) const
{
	float32 tmin, tmax;
	if (!RAABBIntersecter::intersect(ray, region, tmin, tmax))
	{
		return false;
	}
	rec.t = tmax;
	return intersectInternal(ray, rec);
}

bool RTriangleOctree::intersectInternal(const RRay& ray, RIntersectionRecord& rec) const
{
	float32 tmin, tmax;

	if (!RAABBIntersecter::intersect(ray, region, tmin, tmax))
	{
		return false;
	}

	bool found = false;

	float32 mint = 0.f;
	for (const auto& triangle : triangles)
	{
		RIntersectionRecord tempRec;
		if (RTriangleIntersecter::intersect(ray, triangle, tempRec))
		{
			float invTempRecT = 1.f / tempRec.t;
			if (invTempRecT > mint)
			{
				rec = tempRec;
				mint = invTempRecT;
				found = true;
			}
		}
	}

	if (!childNode) return found;

	struct OctantIntersection
	{
		int index;
		float32 t;
	};

	std::array<OctantIntersection, 8> boxIntersections;

	for (int32 i = 0; i < 8; ++i)
	{
		if ((*childNode)[i].region.contains(ray.origin))
		{
			boxIntersections[i].index = i;
			boxIntersections[i].t = 0.f;
		}
		else
		{
			if (RAABBIntersecter::intersect(ray, (*childNode)[i].region, tmin, tmax))
			{
				boxIntersections[i].index = i;
				boxIntersections[i].t = tmin;
			}
			else
			{
				boxIntersections[i].index = -1;
			}
		}
	}

	std::sort(boxIntersections.begin(), boxIntersections.end(),
		[](const OctantIntersection& A, const OctantIntersection& B) -> bool { return A.t < B.t; });

	for (int32 i = 0; i < 8; ++i)
	{
		if (boxIntersections[i].index < 0 || boxIntersections[i].t > rec.t)
		{
			continue;
		}

		if ((*childNode)[boxIntersections[i].index].intersect(ray, rec))
		{
			found = true;
		}
	}

	return found;
}

}