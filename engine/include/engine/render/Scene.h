#pragma once

#include <engine/render/RenderBlock.h>

#include <thread>
#include <iostream>
#include <functional>

namespace render
{

struct Scene
{
public:
	void parallelRender(const math::Camera& camera, const UnsafeOptBuffer& unsafeOptBuffer) const;
public:
	World mworld;
};

}