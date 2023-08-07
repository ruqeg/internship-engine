#include <engine/graphics/LightSystem.h>

namespace graphics
{

void LightSystem::initialize()
{
	HRESULT hr = lightsCB.initialize();
	if (FAILED(hr))
	{
		throw std::exception(("[" + std::to_string(hr) + "] => Failed to create lightsCB.").c_str());
	}
}

void LightSystem::bind(UINT startSlot, UINT mapTarget)
{
	lightsCB.applyChanges(startSlot, mapTarget);
}

void LightSystem::setLightCBS(const LightCBS& lightsCBS)
{
	this->lightsCB.data = lightsCBS;
}

const LightCBS& LightSystem::getLightCBS() const
{
	return lightsCB.data;
}

LightSystem& LightSystem::getInstance()
{
	static LightSystem instance;
	return instance;
}

}