#pragma once

#include <engine/render/hittables/HittableMaterial.h>
#include <engine/math/intersecters/TriangleIntersecter.h>

#include <algorithm>

namespace render
{

class TriangleHittable : public HittableMaterial
{
protected:
	using sharedTriangle_t = std::shared_ptr<math::Triangle>;

public:
	TriangleHittable() = default;
	~TriangleHittable() = default;
	TriangleHittable(const TriangleHittable& other) = default;
	TriangleHittable& operator=(const TriangleHittable& other) = default;
	TriangleHittable(TriangleHittable&& other);
	TriangleHittable& operator=(TriangleHittable&& other);

	explicit TriangleHittable(const sharedTriangle_t& target, const sharedMaterial_t& material);
	explicit TriangleHittable(sharedTriangle_t&& target, sharedMaterial_t&& material);

	void setTriangle(const sharedTriangle_t& newTarget);
	void setTriangle(sharedTriangle_t&& newTarget);
	const sharedTriangle_t& getTriangle() const;

	bool hit(const math::Ray& viewRay, HitRecord& hitRec) const override;

private:
	sharedTriangle_t target;

private:
	static constexpr float epsilon = 0.00001f;
};

}