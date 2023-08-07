#include <engine/graphics/PostProcess.h>

namespace graphics
{

PostProcess& PostProcess::getInstance()
{
	static PostProcess instance;
	return instance;
}

void PostProcess::initialize()
{
	initializeSamplers();

	m_hdrPipelineShaders.vertexShader.initialize(utils::FileSystem::exeFolderPathW() + L"postprocces_vertex.cso", {});
	m_hdrPipelineShaders.pixelShader.initialize(utils::FileSystem::exeFolderPathW() + L"postprocces_hdr_pixel.cso");
	m_fxaaPipelineShaders.vertexShader.initialize(utils::FileSystem::exeFolderPathW() + L"postprocces_vertex.cso", {});
	m_fxaaPipelineShaders.pixelShader.initialize(utils::FileSystem::exeFolderPathW() + L"postprocces_fxaa_pixel.cso");

	m_hdrPostProccesCB.initialize();
	m_fxaaPostProccesCB.initialize();
}

void PostProcess::initializeSamplers()
{
	D3D11_SAMPLER_DESC generalDesc;
	ZeroMemory(&generalDesc, sizeof(D3D11_SAMPLER_DESC));
	generalDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	generalDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	generalDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	generalDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	generalDesc.MinLOD = 0;
	generalDesc.MaxLOD = D3D11_FLOAT32_MAX;

	D3D11_SAMPLER_DESC pointDesc = generalDesc;
	pointDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;
	d3d::device->CreateSamplerState(&pointDesc, m_samplerStatePoint.GetAddressOf());


	D3D11_SAMPLER_DESC bilineerDesc = generalDesc;
	bilineerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3d::device->CreateSamplerState(&bilineerDesc, m_samplerStateBilineer.GetAddressOf());
}

void PostProcess::applyHDR(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& src, Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& dst)
{
	resolve(src, dst, m_hdrPipelineShaders, m_hdrPostProccesCB, m_samplerStatePoint);
}

void PostProcess::applyFXAA(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& src, Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& dst)
{
	resolve(src, dst, m_fxaaPipelineShaders, m_fxaaPostProccesCB, m_samplerStateBilineer);
}

void PostProcess::setGamma(float32 gamma)
{
	m_hdrPostProccesCB.data.gamma = gamma;
}

void PostProcess::setEV100(float32 ev100)
{
	m_hdrPostProccesCB.data.ev100 = ev100;
}

void PostProcess::setQualitySubpix(float32 qualitySubpix)
{
	m_fxaaPostProccesCB.data.qualitySubpix = qualitySubpix;
}

void PostProcess::setQualityEdgeThreshold(float32 qualityEdgeThreshold)
{
	m_fxaaPostProccesCB.data.qualityEdgeThreshold = qualityEdgeThreshold;
}

void PostProcess::setQualityEdgeThresholdMin(float32 qualityEdgeThresholdMin)
{
	m_fxaaPostProccesCB.data.qualityEdgeThresholdMin = qualityEdgeThresholdMin;
}

void PostProcess::setFXAAImageSize(float32 width, float32 height)
{
	m_fxaaPostProccesCB.data.imageSize.x = width;
	m_fxaaPostProccesCB.data.imageSize.y = height;
	m_fxaaPostProccesCB.data.imageSize.z = 1.f / width;
	m_fxaaPostProccesCB.data.imageSize.w = 1.f / height;
}

float32 PostProcess::getGamma() const
{
	return m_hdrPostProccesCB.data.gamma;
}

float32 PostProcess::getEV100() const
{
	return m_hdrPostProccesCB.data.ev100;
}

float32 PostProcess::getQualitySubpix() const
{
	return m_fxaaPostProccesCB.data.qualitySubpix;
}

float32 PostProcess::getQualityEdgeThreshold() const
{
	return m_fxaaPostProccesCB.data.qualityEdgeThreshold;
}

float32 PostProcess::getQualityEdgeThresholdMin() const
{
	return m_fxaaPostProccesCB.data.qualityEdgeThresholdMin;
}

}