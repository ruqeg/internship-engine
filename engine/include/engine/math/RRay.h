#pragma once

#include <engine/types.h>
#include <DirectXMath.h>

namespace math
{

struct RRay
{
public:
	RRay() = default;
	RRay(const DirectX::XMVECTOR& origin, const DirectX::XMVECTOR& direction);
	DirectX::XMVECTOR at(float32 t) const;
public:
	DirectX::XMVECTOR origin;
	DirectX::XMVECTOR direction;
};

}