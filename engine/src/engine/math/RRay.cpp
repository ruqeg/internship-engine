#include <engine/math/RRay.h>

namespace math
{

RRay::RRay(const DirectX::XMVECTOR& origin, const DirectX::XMVECTOR& direction)
	: origin(origin), direction(direction)
{}

DirectX::XMVECTOR RRay::at(float32 t) const
{
	return DirectX::XMVectorAdd(origin, DirectX::XMVectorScale(direction, t));
}

}