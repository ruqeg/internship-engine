#pragma once

#include <engine/render/hittables/HittableMaterial.h>
#include <engine/math/intersecters/SphereIntersecter.h>

#include <glm/geometric.hpp>

#include <algorithm>

namespace render
{

class SphereHittable : public HittableMaterial
{
protected:
	using sharedSphere_t = std::shared_ptr<math::Sphere>;

public:
	SphereHittable() = delete;
	~SphereHittable() = default;
	SphereHittable(const SphereHittable& other) = default;
	SphereHittable& operator=(const SphereHittable& other) = default;
	SphereHittable(SphereHittable&& other);
	SphereHittable& operator=(SphereHittable&& other);

	explicit SphereHittable(const sharedSphere_t& target, const sharedMaterial_t& material);
	explicit SphereHittable(sharedSphere_t&& target, sharedMaterial_t&& material);

	void setSphere(const sharedSphere_t& newTarget);
	void setSphere(sharedSphere_t&& newTarget);
	const sharedSphere_t& getSphere() const;

	bool hit(const math::Ray& viewRay, HitRecord& hitRec) const override;

private:
	sharedSphere_t target;
};

}