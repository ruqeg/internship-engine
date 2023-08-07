#include <engine/render/hittables/MeshGlobalHittable.h>

namespace render
{

MeshGlobalHittable::MeshGlobalHittable(MeshGlobalHittable&& other)
	: HittableMaterial(std::move(other.material)), target(std::move(other.target))
{}

MeshGlobalHittable& MeshGlobalHittable::operator=(MeshGlobalHittable&& other)
{
	this->target = std::move(other.target);
	this->material = std::move(other.material);
	return *this;
}

MeshGlobalHittable::MeshGlobalHittable(const sharedMeshGlobal_t& target, const sharedMaterial_t& material)
	: HittableMaterial(material), target(target)
{}

MeshGlobalHittable::MeshGlobalHittable(sharedMeshGlobal_t&& target, sharedMaterial_t&& material)
	: HittableMaterial(material), target(target)
{}

void MeshGlobalHittable::setMesh(const sharedMeshGlobal_t& newTarget)
{
	target = newTarget;
}

void MeshGlobalHittable::setMesh(sharedMeshGlobal_t&& newTarget)
{
	target = newTarget;
}

bool MeshGlobalHittable::hit(const math::Ray& viewRay, HitRecord& hitRec) const
{
	if (math::MeshGlobalIntersecter::intersect(viewRay, *target, hitRec))
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