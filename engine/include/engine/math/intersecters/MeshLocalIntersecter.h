#pragma once

#include <engine/math/intersecters/TriangleIntersecter.h>

#include <engine/math/MeshLocal.h>

namespace math
{

class MeshLocalIntersecter : public Intersecter
{
public:
	static bool intersect(const Ray& ray, const MeshLocal& meshLocal, IntersectionRecord& rec);
};

}