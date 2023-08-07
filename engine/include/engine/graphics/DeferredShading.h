#pragma once

#include <engine/graphics/ShadingGroup.h>
#include <engine/utils/FileSystem.h>
#include <engine/graphics/PipelineShaders.h>
#include <engine/graphics/GBuffer.h>
#include <engine/utils/ErrorHandler.h>

namespace graphics
{

struct DeferredShadingBindTarget
{
	UINT albedoStartSlot;
	UINT routhnessMetallnessStartSlot;
	UINT normalStartSlot;
	UINT emissionStartSlot;
	UINT depthStartSlot;
	UINT objectIDStartSlot;

	DeferredShadingBindTarget(
		UINT albedoStartSlot,
		UINT routhnessMetallnessStartSlot,
		UINT normalStartSlot,
		UINT emissionStartSlot,
		UINT depthStartSlot,
		UINT objectIDStartSlot);
};

class DeferredShading
{
public:
	static DeferredShading& getInstance();
public:
	void initialize(uint32 windowWidth, uint32 windowHeight);
	void prerender(const std::array<float32, 4>& color);
	template<class T_Instance>
	void renderOpaque(ShadingGroup<T_Instance>& shadingGroup, const ShadingGroupRenderBindTarget& bindTargets);
	template<class T_Instance>
	void renderOpaque(ShadingGroup<T_Instance>& shadingGroup, const ShadingGroupRenderBindTarget& bindTargets, uint32 uavBindIndex, std::vector<ID3D11UnorderedAccessView*> unorderedAccessViews);
	template<class T_Instance>
	void renderEmission(ShadingGroup<T_Instance>& shadingGroup, const ShadingGroupRenderBindTarget& bindTargets);
	void bind(const DeferredShadingBindTarget& bindTarget);
	Microsoft::WRL::ComPtr<ID3D11Texture2D>& getDepthStencilBuffer();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& getDepthSRV();
	GBuffer& getGBuffer();
private:
	void initializeBuffer(uint32 windowWidth, uint32 windowHeight, DXGI_FORMAT format, SRV_RTV_Textuer2D& buffer);
	void initializeDepthBuffer(uint32 windowWidth, uint32 windowHeight);
private:
	DeferredShading() = default;
private:
	GBuffer gbuffer;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> depthShaderResourceView;
private:
	static constexpr DXGI_FORMAT ALBEDO_BUFFER_FORMAT               = DXGI_FORMAT_R8G8B8A8_UNORM;
	static constexpr DXGI_FORMAT ROUTHNESS_METALLNESS_BUFFER_FORMAT = DXGI_FORMAT_R8G8_UNORM;
	static constexpr DXGI_FORMAT NORMAL_BUFFER_FORMAT               = DXGI_FORMAT_R16G16B16A16_SNORM;
	static constexpr DXGI_FORMAT EMISSION_BUFFER_FORMAT             = DXGI_FORMAT_R16G16B16A16_FLOAT; // DXGI_FORMAT_R16G16B16_FLOAT missing
	static constexpr DXGI_FORMAT OBJECT_ID_BUFFER_FORMAT            = DXGI_FORMAT_R32_UINT;
private:
	static constexpr const wchar_t* OPAQUE_PIXEL_SHADER_FILENAME = L"deferred_shading_opaque.cso";
	static constexpr const wchar_t* EMISSION_PIXEL_SHADER_FILENAME = L"deferred_shading_emission.cso";
private:
	static constexpr uint32 GBUFFERS_NUM = 5u;
	static constexpr uint32 ALBEDO_RTV_INDEX = 0u;
	static constexpr uint32 ROUTHNESS_METALLNESS_RTV_INDEX = 1u;
	static constexpr uint32 NORMAL_RTV_INDEX = 2u;
	static constexpr uint32 EMISSION_RTV_INDEX = 3u;
	static constexpr uint32 OBJECTID_RTV_INDEX = 4u;
};

template<class T_Instance>
void DeferredShading::renderOpaque(ShadingGroup<T_Instance>& shadingGroup, const ShadingGroupRenderBindTarget& bindTargets)
{
	std::array<ID3D11RenderTargetView*, GBUFFERS_NUM> RTVs;
	RTVs[ALBEDO_RTV_INDEX] = gbuffer.albedoBuffer.renderTargetView.Get();
	RTVs[ROUTHNESS_METALLNESS_RTV_INDEX] = gbuffer.roughnessMetallnessBuffer.renderTargetView.Get();
	RTVs[NORMAL_RTV_INDEX] = gbuffer.normallBuffer.renderTargetView.Get();
	RTVs[EMISSION_RTV_INDEX] = gbuffer.emissionBuffer.renderTargetView.Get();
	RTVs[OBJECTID_RTV_INDEX] = gbuffer.objectIDBuffer.renderTargetView.Get();

	d3d::devcon->OMSetRenderTargets(RTVs.size(), RTVs.data(), depthStencilView.Get());
	
	shadingGroup.render(bindTargets);
}

template<class T_Instance>
void DeferredShading::renderOpaque(ShadingGroup<T_Instance>& shadingGroup, const ShadingGroupRenderBindTarget& bindTargets, uint32 uavBindIndex, std::vector<ID3D11UnorderedAccessView*> unorderedAccessViews)
{
	std::array<ID3D11RenderTargetView*, GBUFFERS_NUM> RTVs;
	RTVs[ALBEDO_RTV_INDEX] = gbuffer.albedoBuffer.renderTargetView.Get();
	RTVs[ROUTHNESS_METALLNESS_RTV_INDEX] = gbuffer.roughnessMetallnessBuffer.renderTargetView.Get();
	RTVs[NORMAL_RTV_INDEX] = gbuffer.normallBuffer.renderTargetView.Get();
	RTVs[EMISSION_RTV_INDEX] = gbuffer.emissionBuffer.renderTargetView.Get();
	RTVs[OBJECTID_RTV_INDEX] = gbuffer.objectIDBuffer.renderTargetView.Get();

	UINT offset[2] = { -1, -1 };
	d3d::devcon->OMSetRenderTargetsAndUnorderedAccessViews(RTVs.size(), RTVs.data(), depthStencilView.Get(), uavBindIndex, unorderedAccessViews.size(), unorderedAccessViews.data(), offset);

	shadingGroup.render(bindTargets);
}

template<class T_Instance>
void DeferredShading::renderEmission(ShadingGroup<T_Instance>& shadingGroup, const ShadingGroupRenderBindTarget& bindTargets)
{
	d3d::devcon->OMSetRenderTargets(1, gbuffer.emissionBuffer.renderTargetView.GetAddressOf(), depthStencilView.Get());

	shadingGroup.render(bindTargets);
}

}