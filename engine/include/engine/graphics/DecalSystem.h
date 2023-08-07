#pragma once

#include <engine/graphics/PipelineShaders.h>
#include <engine/graphics/VertexBuffer.h>
#include <engine/utils/ModelManager.h>
#include <DirectXMath.h>
#include <engine/utils/FileSystem.h>
#include <engine/utils/ErrorHandler.h>
#include <engine/utils/TextureManager.h>
#include <engine/graphics/MeshSystem.h>

namespace graphics
{

struct DecalInstanceGPU
{
	DirectX::XMFLOAT4X4 decalToWorld;
	DirectX::XMFLOAT4X4 worldToDecal;
	DirectX::XMFLOAT4 color;
	uint32 parentObjectID;
};

struct DecalInstanceCPU
{
	XMMATRIX scaledRotatedDecalBasis;
	XMVECTOR color;
	XMVECTOR offsetInModelSpace;
	utils::SolidVector<MeshSystem::OpaqueInstanceGPU>::ID instanceID;
	uint32 parentObjectID;
	operator DecalInstanceGPU() const;
};

class DecalSystem
{
public:
	static DecalSystem& getInstance();
public:
	void initialize(uint32 w, uint32 h);
	void setDecalInstances(const std::vector<DecalInstanceCPU>& decalInstances);
	std::vector<DecalInstanceCPU>& getDecalInstances();
	void updateInstanceBuffer();
	void render(
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& albedoRTV, 
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& normalRTV, 
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& roughMetallRTV, 
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& emissionRTV,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& objectIDSRV,
		const Microsoft::WRL::ComPtr<ID3D11Texture2D>& depthBuffer, 
		const Microsoft::WRL::ComPtr<ID3D11Texture2D>& normalBuffer, 
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView);
private:
	DecalSystem() = default;
private:
	PipelineShaders m_pipelineShaders;
	std::vector<DecalInstanceCPU> m_decalInstances;
	graphics::VertexBuffer<DecalInstanceGPU> m_instanceBuffer;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_normalCopyBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalCopyShaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> m_depthCopyStencilBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_depthCopyShaderResourceView;
private:
	static constexpr const wchar_t* DECAL_SYSTEM_PIXEL_SHAER = L"decal_system_pixel.cso";
	static constexpr const wchar_t* DECAL_SYSTEM_VERTEX_SHAER = L"decal_system_vertex.cso";
	static constexpr const wchar_t* DECAL_SYSTEM_TEXTURE = L"../resources/textures/splatter-512.dds";
};

}