#pragma once

#include <engine/math/TriangleOctree.h>
#include <vector>

namespace math
{

class MeshLocalIntersecter;

class MeshLocal
{
public:
	MeshLocal() = default;
	MeshLocal(const std::vector<Triangle>& triangles, const AABB& region);
	MeshLocal(const MeshLocal& other);
	MeshLocal& operator=(const MeshLocal& other);
	void setTriangles(const std::vector<Triangle>& newTriangles, const AABB& region);
	inline const std::vector<Triangle>& getTriangles() const { return triangles; }
private:
	std::vector<Triangle> triangles;
	TriangleOctree triangleOctree;

	friend MeshLocalIntersecter;
};

}