#pragma once

#include <engine/types.h>

#include <engine/math/Triangle.h>
#include <engine/math/AABB.h>

#include <engine/math/intersecters/TriangleIntersecter.h>
#include <engine/math/intersecters/AABBIntersecter.h>

#include <algorithm>
#include <memory>
#include <vector>
#include <array>

namespace math
{

class TriangleOctree
{
public:
	TriangleOctree() = default;
	TriangleOctree(const AABB& region);

	TriangleOctree(const TriangleOctree& other);
	TriangleOctree& operator=(const TriangleOctree& other);

	bool build(const std::vector<Triangle>& triangles, const AABB& region);
	bool addTriangle(const Triangle& triangle, const glm::vec3& c);

	bool intersect(const Ray& ray, IntersectionRecord& rec) const;

private:
	bool intersectInternal(const Ray& ray, IntersectionRecord& rec) const;
	bool addTriangleToChildNode(const Triangle& triangle);

private:
	AABB region;
	std::vector<Triangle> triangles;
	std::shared_ptr<std::array<TriangleOctree, 8>> childNode;

private:
	static constexpr int32 prefferedTriangleCount = 32;
	static constexpr glm::vec3 epsilon = { 1e-5f, 1e-5f, 1e-5f };
};

}