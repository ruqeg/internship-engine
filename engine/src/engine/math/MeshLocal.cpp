#include <engine/math/MeshLocal.h>

namespace math
{

MeshLocal::MeshLocal(const MeshLocal& other)
	: triangles(other.triangles), triangleOctree(other.triangleOctree)
{}

MeshLocal& MeshLocal::operator=(const MeshLocal& other)
{
	triangles = other.triangles;
	triangleOctree = other.triangleOctree;
	return *this;
}

MeshLocal::MeshLocal(const std::vector<Triangle>& triangles, const AABB& region)
	: triangles(triangles)
{
	triangleOctree.build(triangles, region);
}

void MeshLocal::setTriangles(const std::vector<Triangle>& newTriangles, const AABB& region)
{
	triangles = newTriangles;
	triangleOctree.build(triangles, region);
}

}