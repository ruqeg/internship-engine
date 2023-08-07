#pragma once

#include <engine/graphics/ShadingGroup.h>
#include <engine/graphics/LightSystem.h>
#include <engine/utils/FileSystem.h>
#include <engine/math/RCamera.h>
#include <engine/graphics/ShadowMapRenderer.hpp>
#include <engine/graphics/DeferredShading.h>

namespace graphics
{

struct MeshInstanceID
{
	uint16 shadingID;
	uint16 modelID;
	uint16 materialID;
	uint16 instanceID;
};

struct ModelInstanceID
{
	std::vector<MeshInstanceID> meshInstanceIDs;
};

class MeshSystem
{
public:
	struct OpaqueInstanceGPU;
	struct DissolutionInstance
	{
		DirectX::XMFLOAT4X4 modelToWorld;
		DirectX::XMFLOAT4 color;
		float32 time;
		explicit operator OpaqueInstanceGPU() const noexcept { return {modelToWorld, color}; }
	};
	struct IncinerationInstance
	{
		DirectX::XMFLOAT4X4 modelToWorld;
		DirectX::XMFLOAT4 color;
		DirectX::XMFLOAT3 rayIntersection;
		float32 time;
		float32 prevBoundingShpereRadius;
		float32 currentBoundingShpereRadius;
		float32 maxBoundingShpereRadius;
		explicit operator OpaqueInstanceGPU() const noexcept { return { modelToWorld, color }; }
	};
	struct OpaqueInstanceGPU
	{
		DirectX::XMFLOAT4X4 modelToWorld;
		DirectX::XMFLOAT4 color;
		uint32 objectID;
		explicit operator DissolutionInstance() const noexcept { return { modelToWorld, color, 0 }; }
	};
public:
	using OpaqueShadingGroup = ShadingGroup<OpaqueInstanceGPU>;
	using DissolutionShadingGroup = ShadingGroup<DissolutionInstance>;
	using IncinerationShadingGroup = ShadingGroup<IncinerationInstance>;
public:
	void render(
		const ShadingGroupRenderBindTarget& bindTargets,
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState>& depthStencilState);
	void render(
		const ShadingGroupRenderBindTarget& bindTargets,
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState>& depthStencilState,
		graphics::PipelineShaders& pipelineShader);
	void initialize();
	void setOpaqueInstances(const OpaqueShadingGroup& opaqueInstances);
	void renderDepth2D(const ShadingGroupRenderBindTarget& bindTargets);
	void renderDepthCubemap(const ShadingGroupRenderBindTarget& bindTargets);
	void updateModelInstancesIDs();
	OpaqueShadingGroup& getOpaqueInstances();
	OpaqueShadingGroup& getDebugOpaqueInstances();
	DissolutionShadingGroup& getDissolutionShadingGroup();
	IncinerationShadingGroup& getIncinerationShadingGroup();
	void remove(const MeshInstanceID& meshInstanceID);
	void remove(uint16 shadingID, const uint32 modelID, const std::vector<uint16> perMeshMaterialIDs, const uint32 instanceID);
public:
	static MeshSystem& getInstance();
private:
	MeshSystem() = default;
	MeshSystem(MeshSystem&) = delete;
	MeshSystem& operator=(MeshSystem&) = delete;
protected:
	// !TODO make vector
	OpaqueShadingGroup opaqueInstances;
	OpaqueShadingGroup debugOpaqueInstances;
	DissolutionShadingGroup dissoultionShadingGroup;
	IncinerationShadingGroup incierationShadingGroup;
	ShadowMapRenderer shadowMapRenderer;
	std::vector<ModelInstanceID> modelInstanceIDs;
};

}
