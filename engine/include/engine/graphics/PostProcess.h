#pragma once

#include <engine/utils/FileSystem.h>
#include <engine/graphics/ShadingGroup.h>

namespace graphics
{

struct HDRPostProccesCBS
{
	float32 gamma;
	DirectX::XMFLOAT3 _pad1;
	float32 ev100;
	DirectX::XMFLOAT3 _pad2;
};

struct FXAAPostProccesCBS
{
	XMFLOAT4 imageSize;
	float32 qualitySubpix;
	XMFLOAT3 _pad1;
	float32 qualityEdgeThreshold;
	XMFLOAT3 _pad2;
	float32 qualityEdgeThresholdMin;
	XMFLOAT3 _pad3;
};


class PostProcess
{
public:
	void initialize();
	void applyHDR(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& src, Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& dst);
	void applyFXAA(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& src, Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& dst);
	void setGamma(float32 gamma);
	void setEV100(float32 ev100);
	void setQualitySubpix(float32 qualitySubpix);
	void setQualityEdgeThreshold(float32 qualityEdgeThreshold);
	void setQualityEdgeThresholdMin(float32 qualityEdgeThresholdMin);
	void setFXAAImageSize(float32 width, float32 height);
	float32 getGamma() const;
	float32 getEV100() const;
	float32 getQualitySubpix() const;
	float32 getQualityEdgeThreshold() const;
	float32 getQualityEdgeThresholdMin() const;
public:
	static PostProcess& getInstance();
private:
	PostProcess() = default;
	PostProcess(PostProcess&) = delete;
	PostProcess& operator=(PostProcess&) = delete;
private:
	template<class T>
	void resolve(
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& src, 
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& dst, 
		graphics::PipelineShaders& pipelineShaders, 
		graphics::ConstantBuffer<T>& constantBuffer,
		Microsoft::WRL::ComPtr<ID3D11SamplerState>& samplerState);
	void initializeSamplers();
private:
	graphics::PipelineShaders m_hdrPipelineShaders;
	graphics::PipelineShaders m_fxaaPipelineShaders;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerStatePoint;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_samplerStateBilineer;
	graphics::ConstantBuffer<HDRPostProccesCBS> m_hdrPostProccesCB;
	graphics::ConstantBuffer<FXAAPostProccesCBS> m_fxaaPostProccesCB;
};

template<class T>
void PostProcess::resolve(
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& src, 
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& dst,
	graphics::PipelineShaders& pipelineShaders, 
	graphics::ConstantBuffer<T>& constantBuffer,
	Microsoft::WRL::ComPtr<ID3D11SamplerState>& samplerState)
{
	d3d::devcon->OMSetRenderTargets(1, dst.GetAddressOf(), nullptr);

	pipelineShaders.bind();

	d3d::devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	constantBuffer.applyChanges(0u, MapTarget::PS);

	d3d::devcon->PSSetSamplers(0u, 1u, samplerState.GetAddressOf());

	d3d::devcon->PSSetShaderResources(0u, 1u, src.GetAddressOf());

	d3d::devcon->Draw(3u, 0u);

	ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
	d3d::devcon->PSSetShaderResources(0u, 1u, nullSRV);
}

}