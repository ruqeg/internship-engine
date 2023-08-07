#include <engine/utils/ColorConverter.h>

namespace utils
{

uint32_t ColorConverter::colorToPixel(glm::vec3 color)
{
	return
		(static_cast<uint32_t>(std::sqrt(std::clamp(color.r, 0.f, 1.f)) * 255.99) & 0x000000FFu) << 16
		| (static_cast<uint32_t>(std::sqrt(std::clamp(color.g, 0.f, 1.f)) * 255.99) & 0x000000FFu) << 8
		| (static_cast<uint32_t>(std::sqrt(std::clamp(color.b, 0.f, 1.f)) * 255.99) & 0x000000FFu);
}

}