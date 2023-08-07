#include <engine/render/World.h>

namespace render
{

bool World::hit(const math::Ray& viewRay, std::shared_ptr<Hittable>& nearestHittable, HitRecord& nearestHitRec) const
{
	nearestHitRec.t = std::numeric_limits<float>::max();
	calcualteNearestHittable(shadingHittables, viewRay, nearestHittable, nearestHitRec);
	calcualteNearestHittable(unshadingHittables, viewRay, nearestHittable, nearestHitRec);
	return nearestHitRec.t != std::numeric_limits<float>::max();
}

void World::calcualteNearestHittable(
	const std::vector<std::shared_ptr<Hittable>>& hittables, const math::Ray& viewRay,
	std::shared_ptr<Hittable>& nearestHittable, HitRecord& nearestHitRec) const
{
	for (const auto& hittable : hittables)
	{
		HitRecord rec;
		if (hittable->hit(viewRay, rec))
		{
			if (rec.t < nearestHitRec.t)
			{
				nearestHittable = hittable;
				nearestHitRec = rec;
			}
		}
	}
}


}