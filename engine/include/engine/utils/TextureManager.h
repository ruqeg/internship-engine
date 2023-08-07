#pragma once

#include <engine/utils/DDSTextureLoader11.h>

#include <engine/graphics/Texture.h>

#include <string>
#include <unordered_map>
#include <memory>
#include <optional>

namespace utils
{

class TextureManager
{
protected:
	using sharedTexture_t = std::shared_ptr<graphics::Texture>;
public:
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;
	void initialize();
	std::optional<sharedTexture_t> getTexture(const std::wstring& filename);
	std::optional<sharedTexture_t> loadTexture(const std::wstring& filename);
public:
	static TextureManager& getInstance();
protected:
	TextureManager() = default;
protected:
	std::unordered_map<std::wstring, sharedTexture_t> map;
};

}