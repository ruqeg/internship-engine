#pragma once

#include <glm/vec3.hpp>
#include <algorithm>
#include <cmath>

namespace utils
{

class ColorConverter
{
public:
	ColorConverter() = delete;
	static uint32_t colorToPixel(glm::vec3 color);
};

}