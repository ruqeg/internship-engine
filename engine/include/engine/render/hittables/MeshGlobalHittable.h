#pragma once

#include <engine/render/hittables/HittableMaterial.h>
#include <engine/math/intersecters/MeshGlobalIntersecter.h>

#include <algorithm>
#include <vector>

namespace render
{

class MeshGlobalHittable : public HittableMaterial
{
protected:
	using sharedMeshGlobal_t = std::shared_ptr<math::MeshGlobal>;

public:
	MeshGlobalHittable() = delete;
	~MeshGlobalHittable() = default;
	MeshGlobalHittable(const MeshGlobalHittable& other) = default;
	MeshGlobalHittable& operator=(const MeshGlobalHittable& other) = default;
	MeshGlobalHittable(MeshGlobalHittable&& other);
	MeshGlobalHittable& operator=(MeshGlobalHittable&& other);

	explicit MeshGlobalHittable(const sharedMeshGlobal_t& target, const sharedMaterial_t& material);
	explicit MeshGlobalHittable(sharedMeshGlobal_t&& target, sharedMaterial_t&& material);

	void setMesh(sharedMeshGlobal_t&& newTarget);
	void setMesh(const sharedMeshGlobal_t& newTarget);
	inline const sharedMeshGlobal_t& getMesh() const;

	bool hit(const math::Ray& viewRay, HitRecord& hitRec) const override;

private:
	sharedMeshGlobal_t target;
};

}

#include <engine/render/hittables/MeshGlobalHittable.inl>