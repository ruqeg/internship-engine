#include <engine/render/hittables/SphereHittable.h>

namespace render
{

SphereHittable::SphereHittable(SphereHittable&& other)
	: HittableMaterial(std::move(material)), target(std::move(other.target))
{}

SphereHittable& SphereHittable::operator=(SphereHittable&& other)
{
	this->target = std::move(other.target);
	this->material = std::move(other.material);
	return *this;
}

SphereHittable::SphereHittable(const sharedSphere_t& target, const sharedMaterial_t& material)
	: HittableMaterial(material), target(target)
{}

SphereHittable::SphereHittable(sharedSphere_t&& target, sharedMaterial_t&& material)
	: HittableMaterial(material), target(target)
{}

void SphereHittable::setSphere(const sharedSphere_t& newTarget)
{
	target = newTarget;
}

void SphereHittable::setSphere(sharedSphere_t&& newTarget)
{
	target = newTarget;
}

const SphereHittable::sharedSphere_t& SphereHittable::getSphere() const
{
	return target;
}

bool SphereHittable::hit(const math::Ray& viewRay, HitRecord& hitRec) const
{
	if (math::SphereIntersecter::intersect(viewRay, *target, hitRec))
	{
		hitRec.material = material;
		return true;
	}
	return false;
}

}