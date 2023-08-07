#pragma once

#include <engine/directx/D3D.h>
#include <DirectXTex/DirectXTex.h>
#include <engine/utils/TextureManager.h>
#include <engine/directx/Macro.h>
#include <engine/graphics/ConstantBuffer.h>
#include <engine/graphics/VertexBuffer.h>
#include <engine/graphics/IndexBuffer.h>
#include <engine/graphics/PipelineShaders.h>
#include <engine/utils/FileSystem.h>
#include <array>
#include <cmath>

#include <DirectXMath.h>

namespace utils
{

class ReflectionCapture
{
private:
	struct WorldToClipCBS
	{
		DirectX::XMFLOAT4X4 worldToClip;
	};
	struct SpecularIrradianceCBS
	{
		DirectX::XMFLOAT4 roughness;
	};
	struct Vertex
	{
		DirectX::XMFLOAT3 position;
	};
public:
	static ReflectionCapture& getInstance();
public:
	HRESULT initialize();
	HRESULT generateDiffuseIrradianceCubemap(
		Microsoft::WRL::ComPtr<ID3D11Resource>& d3d11CubemapResource,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& d3d11CubemapShaderResouceView,
		const uint32 diffuseIrradianceCubemapSize,
		DirectX::ScratchImage& scratchImage);
	HRESULT generateSpecularIrradianceCubemap(
		Microsoft::WRL::ComPtr<ID3D11Resource>& d3d11CubemapResource,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& d3d11CubemapShaderResouceView,
		const uint32 specularIrradianceCubemapSize,
		DirectX::ScratchImage& scratchImage);
	HRESULT generateSpecularReflectance2DTexture(
		const uint32 specularReflectance2DTextureSize,
		DirectX::ScratchImage& scratchImage);
private:
	HRESULT initializeSampler();
	HRESULT initializeWorldToClipMatArr();
	HRESULT initlaizeUnitCube();
	HRESULT initlaizePipeline();
	HRESULT createRenderCubemapTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3d11Texture2d, const uint32 size, const uint32 mipLevels = 1u);
	HRESULT createRenderTexture2D(Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3d11Texture2d, const uint32 size);
	HRESULT initializeDiffuseIrradianceRTVs(
		Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3d11CubemapTexture2D,
		std::array<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, 6>& d3d11RTVs);
	HRESULT initializeSpecularIrradianceRTV(
		Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3d11CubemapTexture2D,
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& d3d11RTV,
		uint32 arraySlice,
		uint32 maxMipmapLevels);
	HRESULT initializeViewport(D3D11_VIEWPORT& viewport, const uint32 size);
private:
	ReflectionCapture() = default;
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_d3d11Sampler;
	graphics::ConstantBuffer<WorldToClipCBS> m_worldToClipCB;
	graphics::ConstantBuffer<SpecularIrradianceCBS> m_specularIrradianceCB;
	std::array<DirectX::XMMATRIX, 6> m_worlToClipMatArr;
	graphics::VertexBuffer<Vertex> m_unitCubeVertexBuffer;
	graphics::IndexBuffer m_unitCubeIndexBuffer;
	graphics::PipelineShaders m_specularIrradianceCubemapPipelineShaders;
	graphics::PipelineShaders m_diffuseIrradianceCubemapPipelineShaders;
	graphics::PipelineShaders m_specularReflectance2DTexturePipelineShaders;
private:
	static constexpr DXGI_FORMAT m_d3d11TexFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
	static constexpr const wchar_t* m_diffVertexShaderPathW = L"diffuse_irradiance_cubemap_vertex.cso";
	static constexpr const wchar_t* m_diffPixelShaderPathW = L"diffuse_irradiance_cubemap_pixel.cso";
	static constexpr const wchar_t* m_specVertexShaderPathW = L"specular_irradiance_cubemap_vertex.cso";
	static constexpr const wchar_t* m_specPixelShaderPathW = L"specular_irradiance_cubemap_pixel.cso";
	static constexpr const wchar_t* m_specReflectanceVertexShaderPathW = L"specular_reflectance_texture2d_vertex.cso";
	static constexpr const wchar_t* m_specReflectancePixelShaderPathW = L"specular_reflectance_texture2d_pixel.cso";
};


} //namespace utils