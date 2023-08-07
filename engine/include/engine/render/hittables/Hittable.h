#pragma once

#include <engine/math/intersecters/Intersecter.h>
#include <engine/render/Material.h>

#include <memory>

namespace render
{

struct HitRecord : public math::IntersectionRecord
{
	std::shared_ptr<Material> material;
};

class Hittable
{
protected:
	using sharedMaterial_t = std::shared_ptr<Material>;
public:
	virtual bool hit(const math::Ray& viewRay, HitRecord& hitRec) const = 0;
};

}