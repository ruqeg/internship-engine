#include <engine/utils/TextureManager.h>

namespace utils
{

void TextureManager::initialize()
{
	//TODO
}

TextureManager& TextureManager::getInstance()
{
	static TextureManager instance;
	return instance;
}

std::optional<TextureManager::sharedTexture_t> TextureManager::getTexture(const std::wstring& filename)
{
	if (const auto search = map.find(filename); search != map.end())
	{
		return search->second;
	}
	else
	{
		std::optional<std::shared_ptr<graphics::Texture>> texture = loadTexture(filename);
		if (texture.has_value())
		{
			map[filename] = texture.value();
		}
		return texture;
	}
}

std::optional<TextureManager::sharedTexture_t> TextureManager::loadTexture(const std::wstring& filename)
{
	graphics::Texture texture;

	HRESULT hr = DirectX::CreateDDSTextureFromFile(
		d3d::device,
		filename.c_str(),
		texture.resource.GetAddressOf(),
		texture.shaderResourceView.GetAddressOf());

	if (FAILED(hr))
	{
		return std::nullopt;
	}

	return std::make_shared<graphics::Texture>(texture);
}

}