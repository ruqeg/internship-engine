#include <engine/math/TriangleOctree.h>

namespace math
{

TriangleOctree::TriangleOctree(const TriangleOctree& other)
	: region(other.region), triangles(other.triangles), childNode(other.childNode)
{}

TriangleOctree& TriangleOctree::operator=(const TriangleOctree& other)
{
	region = other.region;
	triangles = other.triangles;
	childNode = other.childNode;
	return *this;
}

TriangleOctree::TriangleOctree(const AABB& region)
	: region(AABB(region.min - epsilon, region.max + epsilon))
{}

bool TriangleOctree::build(const std::vector<Triangle>& newTriangles, const AABB& newRegion)
{
	triangles.clear();
	triangles.shrink_to_fit();

	childNode = nullptr;

	region = AABB(newRegion.min - epsilon, newRegion.max + epsilon);

	for (const auto& triangle : newTriangles)
	{
		const glm::vec3 tc = (triangle.p0 + triangle.p1 + triangle.p2) / 3.f;
		if (!addTriangle(triangle, tc))
		{
			return false;
		}
	}
	return true;
}

bool TriangleOctree::addTriangle(const Triangle& triangle, const glm::vec3& c)
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
			const glm::vec3 mn = region.min;
			const glm::vec3 mx = region.max;
			const glm::vec3 c = (region.min + region.max) / 2.f;

			childNode.reset(new std::array<TriangleOctree, 8>());
			(*childNode)[0] = TriangleOctree(AABB(mn, c));
			(*childNode)[1] = TriangleOctree(AABB({ c.x, mn.y, mn.z }, { mx.x, c.y, c.z }));
			(*childNode)[2] = TriangleOctree(AABB({ c.x, mn.y, c.z }, { mx.x, c.y, mx.z }));
			(*childNode)[3] = TriangleOctree(AABB({ mn.x, mn.y, c.z }, { c.x, c.y, mx.z }));
			(*childNode)[4] = TriangleOctree(AABB({ mn.x, c.y, mn.z }, { c.x, mx.y, c.z }));
			(*childNode)[5] = TriangleOctree(AABB({ c.x, c.y, mn.z }, { mx.x, mx.y, c.z }));
			(*childNode)[6] = TriangleOctree(AABB(c, mx));
			(*childNode)[7] = TriangleOctree(AABB({ mn.x, c.y, c.z }, { c.x, mx.y, mx.z }));

			std::vector<Triangle> newTriangles;

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

bool TriangleOctree::addTriangleToChildNode(const Triangle& triangle)
{
	const glm::vec3 triangleCenter = (triangle.p0 + triangle.p1 + triangle.p2) / 3.f;

	for (int32 i = 0; i < 8; ++i)
	{
		if ((*childNode)[i].addTriangle(triangle, triangleCenter))
		{
			return true;
		}
	}

	return false;
}

bool TriangleOctree::intersect(const Ray& ray, IntersectionRecord& rec) const
{
	float32 tmin, tmax;
	if (!AABBIntersecter::intersect(ray, region, tmin, tmax))
	{
		return false;
	}
	rec.t = tmax;
	return intersectInternal(ray, rec);
}

bool TriangleOctree::intersectInternal(const Ray& ray, IntersectionRecord& rec) const
{
	float32 tmin, tmax;

	if (!AABBIntersecter::intersect(ray, region, tmin, tmax))
	{
		return false;
	}

	bool found = false;

	float32 mint = 0.f;
	for (const auto& triangle : triangles)
	{
		IntersectionRecord tempRec;
		if (TriangleIntersecter::intersect(ray, triangle, tempRec))
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
			if (AABBIntersecter::intersect(ray, (*childNode)[i].region, tmin, tmax))
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