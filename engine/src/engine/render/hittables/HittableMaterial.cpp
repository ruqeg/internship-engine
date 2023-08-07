#include <engine/render/hittables/PlaneHittable.h>

namespace render
{

HittableMaterial::HittableMaterial(const sharedMaterial_t& material)
	: material(material)
{}

HittableMaterial::HittableMaterial(sharedMaterial_t&& material)
	: material(material)
{}

void HittableMaterial::setMaterial(const sharedMaterial_t& newMaterial)
{
	material = newMaterial;
}

const HittableMaterial::sharedMaterial_t& HittableMaterial::getMaterial() const
{
	return material;
}

}