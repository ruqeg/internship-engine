#pragma once

#include <DirectXMath.h>
#include <engine/types.h>

namespace graphics
{

struct Particle
{
	DirectX::XMVECTOR tint_alpha;
	DirectX::XMVECTOR position;
	float32 rotationAngle;
	DirectX::XMVECTOR size;
	DirectX::XMVECTOR constantSpeed;
	float64 time;
};

}