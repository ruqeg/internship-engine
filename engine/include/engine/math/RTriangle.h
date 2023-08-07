#pragma once

#include <DirectXMath.h>

namespace math
{

struct RTriangle
{
public:
	RTriangle() = default;
	RTriangle(const DirectX::XMVECTOR& p0, const DirectX::XMVECTOR& p1, const DirectX::XMVECTOR& p2) : p0(p0), p1(p1), p2(p2) {}
public:
	DirectX::XMVECTOR p0;
	DirectX::XMVECTOR p1;
	DirectX::XMVECTOR p2;
};

}