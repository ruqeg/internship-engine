#pragma once

#include <engine/graphics/ShadingGroup.h>
#include <engine/graphics/ConstantBuffer.h>
#include <engine/utils/FileSystem.h>
#include <engine/math/RCamera.h>
#include <engine/graphics/PipelineShaders.h>

#include <memory>

struct FullscrenTriangleNearPlaneConstantBufferStruct
{
	XMFLOAT4 nearLB_LT_RB[3];
};

class FullscreenTriangle
{
public:
	void initialize();
	void setTexture(const std::shared_ptr<graphics::Texture>& cubeTexture);
	void update(int32 shaderRegister, const math::RCamera& camera);
	void render(UINT textureStartSlot);
private:
	graphics::ConstantBuffer<FullscrenTriangleNearPlaneConstantBufferStruct> frustumConstanstBuffer;
	std::shared_ptr<graphics::Texture> cubeTexture;
	graphics::PipelineShaders pipelineShaders;
};