#include <engine/graphics/FullscreenPass.h>

namespace graphics
{

FullscreenPass& FullscreenPass::getInstance()
{
	static FullscreenPass instance;
	return instance;
}


void FullscreenPass::initialize()
{
	pbrPipelineShaders.vertexShader.initialize(utils::FileSystem::exeFolderPathW() + vertexShaderFilename, {});
	pbrPipelineShaders.pixelShader.initialize(utils::FileSystem::exeFolderPathW() + pixelShaderPBRFilename);
	emissionPipelineShaders.vertexShader.initialize(utils::FileSystem::exeFolderPathW() + vertexShaderFilename, {});
	emissionPipelineShaders.pixelShader.initialize(utils::FileSystem::exeFolderPathW() + pixelShaderEmissionFilename);
}

void FullscreenPass::renderPBR()
{
	render(pbrPipelineShaders);
}

void FullscreenPass::renderEmission()
{
	render(emissionPipelineShaders);
}

void FullscreenPass::render(graphics::PipelineShaders& pipelineShaders)
{
	pipelineShaders.bind();
	d3d::devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	d3d::devcon->Draw(3u, 0u);
}

const graphics::DeferredShadingBindTarget& FullscreenPass::getDeferredBindTarget()
{
	return defferedBindTarget;
}

}