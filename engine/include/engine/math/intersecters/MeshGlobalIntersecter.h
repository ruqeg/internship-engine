#pragma once

#include <engine/math/intersecters/MeshLocalIntersecter.h>

#include <engine/math/MeshGlobal.h>

namespace math
{

class MeshGlobalIntersecter : public Intersecter
{
public:
	static bool intersect(const Ray& ray, const MeshGlobal& meshGlobal, IntersectionRecord& rec);
};

}