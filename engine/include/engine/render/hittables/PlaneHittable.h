#pragma once

#include <engine/render/hittables/HittableMaterial.h>
#include <engine/math/intersecters/PlaneIntersecter.h>

#include <glm/geometric.hpp>

#include <algorithm>

namespace render
{

class PlaneHittable : public HittableMaterial
{
protected:
	using sharedPlane_t = std::shared_ptr<math::Plane>;

public:
	PlaneHittable() = delete;
	~PlaneHittable() = default;
	PlaneHittable(const PlaneHittable& other) = default;
	PlaneHittable& operator=(const PlaneHittable& other) = default;
	PlaneHittable(PlaneHittable&& other);
	PlaneHittable& operator=(PlaneHittable&& other);

	explicit PlaneHittable(const sharedPlane_t& target, const sharedMaterial_t& material);
	explicit PlaneHittable(sharedPlane_t&& target, sharedMaterial_t&& material);

	void setPlane(const sharedPlane_t& newTarget);
	void setPlane(sharedPlane_t&& newTarget);
	const sharedPlane_t& getPlane() const;

	bool hit(const math::Ray& viewRay, HitRecord& hitRec) const override;

private:
	sharedPlane_t target;
};

}