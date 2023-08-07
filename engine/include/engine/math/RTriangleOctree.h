#pragma once

#include <engine/math/RAABB.h>

#include <engine/math/intersecters/RTriangleIntersecter.h>
#include <engine/math/intersecters/RAABBIntersecter.h>

#include <algorithm>
#include <memory>
#include <vector>
#include <array>

namespace math
{

class RTriangleOctree
{
public:
	RTriangleOctree() = default;
	RTriangleOctree(const RAABB& region);

	RTriangleOctree(const RTriangleOctree& other);
	RTriangleOctree& operator=(const RTriangleOctree& other);

	bool build(const std::vector<RTriangle>& triangles, const RAABB& region);
	bool addTriangle(const RTriangle& triangle, const DirectX::XMVECTOR& c);

	bool intersect(const RRay& ray, RIntersectionRecord& rec) const;

private:
	bool intersectInternal(const RRay& ray, RIntersectionRecord& rec) const;
	bool addTriangleToChildNode(const RTriangle& triangle);

private:
	RAABB region;
	std::vector<RTriangle> triangles;
	std::shared_ptr<std::array<RTriangleOctree, 8>> childNode;

private:
	static constexpr int32 prefferedTriangleCount = 32;
	static constexpr DirectX::XMVECTOR epsilon = { 1e-5f, 1e-5f, 1e-5f };
};

}