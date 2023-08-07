#include <engine/math/RAABB.h>

using namespace DirectX;

namespace math
{

RAABB::RAABB(const DirectX::XMVECTOR& min, const DirectX::XMVECTOR& max)
	: min(min), max(max)
{}

bool RAABB::contains(const DirectX::XMVECTOR& point) const
{
	return
		XMVectorGetX(point) >= XMVectorGetX(min) && XMVectorGetX(point) <= XMVectorGetX(max) &&
		XMVectorGetY(point) >= XMVectorGetY(min) && XMVectorGetY(point) <= XMVectorGetY(max) &&
		XMVectorGetZ(point) >= XMVectorGetZ(min) && XMVectorGetZ(point) <= XMVectorGetZ(max);
}

}