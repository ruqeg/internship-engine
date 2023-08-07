#pragma once

#include <engine/types.h>
#include <DirectXMath.h>

namespace math
{

struct IndexTriangle
{
public:
	IndexTriangle() = default;
	IndexTriangle(uint32 i0, uint32 i1, uint32 i2) : i0(i0), i1(i1), i2(i2) {}
public:
	uint32 i0;
	uint32 i1;
	uint32 i2;
};

}