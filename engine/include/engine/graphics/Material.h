#pragma once

#include <DirectXMath.h>
#include <engine/graphics/Texture.h>
#include <memory>

namespace graphics
{

struct Material
{
	std::shared_ptr<Texture> albedoMap;
	std::shared_ptr<Texture> metallicMap;
	std::shared_ptr<Texture> roughnessMap;
	std::shared_ptr<Texture> normalMap;

	//TODO move to cpp
	inline bool operator==(const Material& other) const
	{
		bool byte1Ok = this->albedoMap == other.albedoMap;
		bool byte2Ok = this->metallicMap == other.metallicMap;
		bool byte3Ok = this->roughnessMap == other.roughnessMap;
		bool byte4Ok = this->normalMap == other.normalMap;
		return byte1Ok && byte2Ok && byte3Ok && byte4Ok;
	}
	inline bool operator!=(const Material& other) const
	{
		bool byte1Ok = this->albedoMap != other.albedoMap;
		bool byte2Ok = this->metallicMap != other.metallicMap;
		bool byte3Ok = this->roughnessMap != other.roughnessMap;
		bool byte4Ok = this->normalMap != other.normalMap;
		return byte1Ok || byte2Ok || byte3Ok || byte4Ok;
	}
};

}