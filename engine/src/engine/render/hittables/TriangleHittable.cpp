#include <engine/render/hittables/TriangleHittable.h>

namespace render
{

TriangleHittable::TriangleHittable(TriangleHittable&& other)
	: HittableMaterial(std::move(material)), target(std::move(other.target))
{}

TriangleHittable& TriangleHittable::operator=(TriangleHittable&& other)
{
	this->material = std::move(other.material);
	this->target = std::move(other.target);
	return *this;
}

TriangleHittable::TriangleHittable(const sharedTriangle_t& target, const sharedMaterial_t& material)
	: HittableMaterial(material), target(target)
{}

TriangleHittable::TriangleHittable(sharedTriangle_t&& target, sharedMaterial_t&& material)
	: HittableMaterial(material), target(target)
{}

void TriangleHittable::setTriangle(const sharedTriangle_t& newTarget)
{
	target = newTarget;
}

void TriangleHittable::setTriangle(sharedTriangle_t&& newTarget)
{
	target = newTarget;
}

const TriangleHittable::sharedTriangle_t& TriangleHittable::getTriangle() const
{
	return target;
}

bool TriangleHittable::hit(const math::Ray& viewRay, HitRecord& hitRec) const
{
	if (math::TriangleIntersecter::intersect(viewRay, *target, hitRec))
	{
		hitRec.material = material;
		if (glm::dot(hitRec.normal, viewRay.direction) > 0.f)
		{
			hitRec.normal = -hitRec.normal;
		}
		return true;
	}
	return false;
}

}