#include <engine/render/hittables/PlaneHittable.h>

namespace render
{

PlaneHittable::PlaneHittable(PlaneHittable&& other)
	: HittableMaterial(std::move(material)), target(std::move(target))
{}

PlaneHittable& PlaneHittable::operator=(PlaneHittable&& other)
{
	this->target = std::move(other.target);
	this->material = std::move(other.material);
	return *this;
}

PlaneHittable::PlaneHittable(const sharedPlane_t& target, const sharedMaterial_t& material)
	: HittableMaterial(material), target(target)
{}

PlaneHittable::PlaneHittable(sharedPlane_t&& target, sharedMaterial_t&& material)
	: HittableMaterial(material), target(target)
{}

void PlaneHittable::setPlane(const sharedPlane_t& newTarget)
{
	target = newTarget;
}

void PlaneHittable::setPlane(sharedPlane_t&& newTarget)
{
	target = newTarget;
}

const PlaneHittable::sharedPlane_t& PlaneHittable::getPlane() const
{
	return target;
}

bool PlaneHittable::hit(const math::Ray& viewRay, HitRecord& hitRec) const
{
	if (math::PlaneIntersecter::intersect(viewRay, *target, hitRec))
	{
		hitRec.material = material;
		if (glm::dot(viewRay.direction, hitRec.normal) > 0)
		{
			hitRec.normal -= hitRec.normal;
		}
		return true;
	}
	return false;
}

}