#pragma once

#include <DirectXMath.h>

namespace math
{

struct RAABB
{
public:
	RAABB() = default;
	RAABB(const DirectX::XMVECTOR& min, const DirectX::XMVECTOR& max);
	bool contains(const DirectX::XMVECTOR& point) const;
public:
	DirectX::XMVECTOR min;
	DirectX::XMVECTOR max;
};

}