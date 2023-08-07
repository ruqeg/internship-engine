#pragma once

#include <engine/types.h>
#include <engine/math/Ray.h>

namespace math
{

struct IntersectionRecord
{
	glm::vec3 normal;
	glm::vec3 position;
	float32 t;
};

class Intersecter
{
protected:
	static constexpr float32 epsilon = 0.00001f;
};

}