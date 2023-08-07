#pragma once

#include <engine/graphics/DeferredShading.h>

namespace graphics
{

class FullscreenPass
{
public:
	static FullscreenPass& getInstance();
public:
	void initialize();
	void renderPBR();
	void renderEmission();
	const graphics::DeferredShadingBindTarget& getDeferredBindTarget();
private:
	void render(graphics::PipelineShaders& pipelineShaders);
private:
	graphics::PipelineShaders pbrPipelineShaders;
	graphics::PipelineShaders emissionPipelineShaders;
private:
	const graphics::DeferredShadingBindTarget defferedBindTarget = graphics::DeferredShadingBindTarget(0u, 1u, 2u, 3u, 4u, 5u);
private:
	FullscreenPass() = default;
private:
	static constexpr const wchar_t* vertexShaderFilename = L"fullscreen_pass_vertex.cso";
	static constexpr const wchar_t* pixelShaderPBRFilename = L"fullscreen_pass_pbr_pixel.cso";
	static constexpr const wchar_t* pixelShaderEmissionFilename = L"fullscreen_pass_emission_pixel.cso";
};

}