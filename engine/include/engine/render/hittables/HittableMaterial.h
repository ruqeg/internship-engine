#pragma once

#include <engine/render/hittables/Hittable.h>

namespace render
{

class HittableMaterial : public Hittable
{
protected:
	using sharedMaterial_t = std::shared_ptr<Material>;
public:
	HittableMaterial(const sharedMaterial_t& material);
	HittableMaterial(sharedMaterial_t&& material);
	void setMaterial(const sharedMaterial_t& newMaterial);
	const sharedMaterial_t& getMaterial() const;
protected:
	sharedMaterial_t material;
};

}