#pragma once

#include <engine/directx/D3D.h>
#include <engine/graphics/ConstantBuffer.h>
#include <DirectXMath.h>

namespace graphics
{

struct DirectionalLightsCBS
{
	DirectX::XMFLOAT4 direction;
	DirectX::XMFLOAT4 intensity;
	DirectX::XMFLOAT4 solidAngle;
};

struct PointLightsCBS
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 intensity;
	DirectX::XMFLOAT4 radius;
};

struct SpotLightCBS
{
	DirectX::XMFLOAT4 position;
	DirectX::XMFLOAT4 intensity;
	DirectX::XMFLOAT4 radius;
	DirectX::XMFLOAT4 direction;
	DirectX::XMFLOAT4 angle;
};

struct LightCBS
{
	static constexpr int32 maxCount = 4;
	DirectionalLightsCBS directionalLightsCBS[maxCount];
	PointLightsCBS pointLightCBS[maxCount];
	SpotLightCBS spotLightsCBS[maxCount];
	DirectX::XMUINT4 pointLightsSize;
	DirectX::XMUINT4 directionalLightsSize;
	DirectX::XMUINT4 spotLightsSize;
};

class LightSystem
{
public:
	void initialize();
	void bind(UINT startSlot, UINT mapTarget);

	void setLightCBS(const LightCBS& lightsCBS);
	const LightCBS& getLightCBS() const;
public:
	static LightSystem& getInstance();
private:
	LightSystem() = default;
	LightSystem(LightSystem&) = delete;
	LightSystem& operator=(LightSystem&) = delete;
private:
	graphics::ConstantBuffer<LightCBS> lightsCB;
};

}