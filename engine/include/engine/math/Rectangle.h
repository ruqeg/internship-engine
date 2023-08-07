#pragma once

#include <engine/types.h>

#include <glm/vec2.hpp>

namespace math
{

struct Rectangle
{
	glm::vec2 pos;
	glm::vec2 size;
};

struct IntRectangle
{
	glm::ivec2 pos;
	glm::ivec2 size;
};

struct IntRectanglePrimitive
{
	int32 x;
	int32 y;
	int32 w;
	int32 h;
};

}