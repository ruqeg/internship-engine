#pragma once

#include <engine/render/hittables/Hittable.h>
#include <engine/render/lights/LightSource.h>

#include <memory>
#include <vector>

namespace render
{

struct World
{
protected:
	using sharedHittableVecIt_t = std::vector<std::shared_ptr<Hittable>>::iterator;
public:
	bool hit(const math::Ray& viewRay, std::shared_ptr<Hittable>& nearestHittable, HitRecord& nearestHitRec) const;

private:
	void calcualteNearestHittable(
		const std::vector<std::shared_ptr<Hittable>>& hittables, const math::Ray& viewRay,
		std::shared_ptr<Hittable>& nearestHittable, HitRecord& nearestHitRec) const;

public:
	std::vector<std::shared_ptr<Hittable>> shadingHittables;
	std::vector<std::shared_ptr<Hittable>> unshadingHittables;
	std::vector<std::shared_ptr<LightSource>> lightsSources;

public:
	glm::vec3 backgroundColor;
};

}