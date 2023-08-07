#pragma once

#include <engine/types.h>
#include <engine/math/RRay.h>

namespace math
{

struct RIntersectionRecord
{
	DirectX::XMVECTOR normal;
	DirectX::XMVECTOR position;
	float32 t;
};

class Intersecter
{
protected:
	static constexpr float32 epsilon = 0.00001f;
};

}