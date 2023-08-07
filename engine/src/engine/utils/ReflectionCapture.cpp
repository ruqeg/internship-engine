#include <engine/utils/ReflectionCapture.h>

namespace utils
{

ReflectionCapture& ReflectionCapture::getInstance()
{
	static ReflectionCapture instance;
	return instance;
}

HRESULT ReflectionCapture::initialize()
{
	RETURN_IF_FAILED(initializeSampler());
	RETURN_IF_FAILED(m_worldToClipCB.initialize());
	RETURN_IF_FAILED(m_specularIrradianceCB.initialize());
	RETURN_IF_FAILED(initializeWorldToClipMatArr());
	RETURN_IF_FAILED(initlaizeUnitCube());
	RETURN_IF_FAILED(initlaizePipeline());
	return S_OK;
}

HRESULT ReflectionCapture::initializeSampler()
{
	D3D11_SAMPLER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
	desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	desc.MinLOD = 0;
	desc.MaxLOD = D3D11_FLOAT32_MAX;

	desc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_POINT;

	HRESULT hr = d3d::device->CreateSamplerState(&desc, m_d3d11Sampler.GetAddressOf());

	return hr;
}

HRESULT ReflectionCapture::initializeWorldToClipMatArr()
{
	const std::array<DirectX::XMMATRIX, 6> worlToViewMatArr =
	{
		DirectX::XMMatrixLookAtLH({0.f, 0.f, 0.f}, { 1.f, 0.f, 0.f}, { 0.f, 1.f, 0.f}),
		DirectX::XMMatrixLookAtLH({0.f, 0.f, 0.f}, {-1.f, 0.f, 0.f}, { 0.f, 1.f, 0.f}),
		DirectX::XMMatrixLookAtLH({0.f, 0.f, 0.f}, { 0.f, 1.f, 0.f}, { 0.f, 0.f,-1.f}),
		DirectX::XMMatrixLookAtLH({0.f, 0.f, 0.f}, { 0.f,-1.f, 0.f}, { 0.f, 0.f, 1.f}),
		DirectX::XMMatrixLookAtLH({0.f, 0.f, 0.f}, { 0.f, 0.f, 1.f}, { 0.f, 1.f, 0.f}),
		DirectX::XMMatrixLookAtLH({0.f, 0.f, 0.f}, { 0.f, 0.f,-1.f}, { 0.f, 1.f, 0.f})
	};

	const DirectX::XMMATRIX viewToClipMat 
		= DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV2, 1.f, 0.1f, 2.f);
	
	for (size_t i = 0; i < 6; ++i)
	{
		m_worlToClipMatArr[i] = DirectX::XMMatrixMultiply(worlToViewMatArr[i], viewToClipMat);
	}

	return S_OK;
}

HRESULT ReflectionCapture::initlaizeUnitCube()
{
	const std::vector<Vertex> vertexBufferData =
	{
		{ { 1.f,  1.f, -1.f }},
		{ {-1.f,  1.f, -1.f }},
		{ { 1.f, -1.f, -1.f }},
		{ {-1.f, -1.f, -1.f }},

		{ { 1.f,  1.f,  1.f } },
		{ {-1.f,  1.f,  1.f } },
		{ { 1.f, -1.f,  1.f } },
		{ {-1.f, -1.f,  1.f } }
	};
	const std::vector<DWORD> indexBufferData =
	{
		2, 0, 1,
		3, 2, 1,
		
		1, 5, 3,
		5, 7, 3,
		
		4, 0, 2,
		6, 4, 2,

		7, 5, 6,
		5, 4, 6,

		4, 5, 1,
		0, 4, 1,

		6, 2, 3,
		7, 6, 3,
	};

	RETURN_IF_FAILED(m_unitCubeVertexBuffer.initialize(vertexBufferData.data(), vertexBufferData.size()));
	RETURN_IF_FAILED(m_unitCubeIndexBuffer.initialize(indexBufferData.data(), indexBufferData.size()));

	return S_OK;
}

HRESULT ReflectionCapture::initlaizePipeline()
{
	const std::vector<D3D11_INPUT_ELEMENT_DESC> shaderInputLayoutDesc =
	{
		{ "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	m_diffuseIrradianceCubemapPipelineShaders.vertexShader.initialize(utils::FileSystem::exeFolderPathW() + m_diffVertexShaderPathW, shaderInputLayoutDesc);
	m_diffuseIrradianceCubemapPipelineShaders.pixelShader.initialize(utils::FileSystem::exeFolderPathW() + m_diffPixelShaderPathW);
	m_specularIrradianceCubemapPipelineShaders.vertexShader.initialize(utils::FileSystem::exeFolderPathW() + m_specVertexShaderPathW, shaderInputLayoutDesc);
	m_specularIrradianceCubemapPipelineShaders.pixelShader.initialize(utils::FileSystem::exeFolderPathW() + m_specPixelShaderPathW);
	m_specularReflectance2DTexturePipelineShaders.vertexShader.initialize(utils::FileSystem::exeFolderPathW() + m_specReflectanceVertexShaderPathW, {});
	m_specularReflectance2DTexturePipelineShaders.pixelShader.initialize(utils::FileSystem::exeFolderPathW() + m_specReflectancePixelShaderPathW);
	return S_OK;
}

HRESULT ReflectionCapture::generateDiffuseIrradianceCubemap(
	Microsoft::WRL::ComPtr<ID3D11Resource>& d3d11CubemapResource,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& d3d11CubemapShaderResouceView,
	const uint32 diffuseIrradianceCubemapSize,
	DirectX::ScratchImage& scratchImage)
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> diffuseIrradianceCubemap;
	RETURN_IF_FAILED(createRenderCubemapTexture(diffuseIrradianceCubemap, diffuseIrradianceCubemapSize));

	std::array<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, 6> d3d11RTVs;
	RETURN_IF_FAILED(initializeDiffuseIrradianceRTVs(diffuseIrradianceCubemap, d3d11RTVs));

	D3D11_VIEWPORT viewport;
	RETURN_IF_FAILED(initializeViewport(viewport, diffuseIrradianceCubemapSize));

	d3d::devcon->RSSetState(nullptr);
	d3d::devcon->OMSetDepthStencilState(nullptr, 0);
	d3d::devcon->RSSetViewports(1, &viewport);

	for (UINT i = 0; i < 6; ++i)
	{
		auto d3d11RTV = d3d11RTVs[i];

		d3d::devcon->OMSetRenderTargets(1, d3d11RTV.GetAddressOf(), nullptr);

		FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		d3d::devcon->ClearRenderTargetView(d3d11RTV.Get(), clearColor);

		d3d::devcon->PSSetShaderResources(0, 1, d3d11CubemapShaderResouceView.GetAddressOf());
		d3d::devcon->PSSetSamplers(0, 1, m_d3d11Sampler.GetAddressOf());

		DirectX::XMStoreFloat4x4(&m_worldToClipCB.data.worldToClip, m_worlToClipMatArr[i]);
		m_worldToClipCB.applyChanges(0, graphics::MapTarget::VS);

		m_unitCubeIndexBuffer.bind(0u);
		m_unitCubeVertexBuffer.bind(0u, 1u, 0u);

		m_diffuseIrradianceCubemapPipelineShaders.bind();

		d3d::devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

		d3d::devcon->DrawIndexed(36, 0, 0);
	}

	DirectX::CaptureTexture(d3d::device, d3d::devcon, diffuseIrradianceCubemap.Get(), scratchImage);
	return S_OK;
}

HRESULT ReflectionCapture::generateSpecularIrradianceCubemap(
	Microsoft::WRL::ComPtr<ID3D11Resource>& d3d11CubemapResource,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& d3d11CubemapShaderResouceView,
	const uint32 specularIrradianceCubemapSize,
	DirectX::ScratchImage& scratchImage)
{
	uint32 maxMipmapLevel = std::log2f(specularIrradianceCubemapSize) + 1;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> specularIrradianceCubemap;
	RETURN_IF_FAILED(createRenderCubemapTexture(specularIrradianceCubemap, specularIrradianceCubemapSize, maxMipmapLevel));

	d3d::devcon->RSSetState(nullptr);
	d3d::devcon->OMSetDepthStencilState(nullptr, 0);

	for (UINT mipmapLevel = 0; mipmapLevel < maxMipmapLevel; ++mipmapLevel)
	{
		D3D11_VIEWPORT viewport;
		RETURN_IF_FAILED(initializeViewport(viewport, specularIrradianceCubemapSize * std::powf(0.5f, mipmapLevel)));
		d3d::devcon->RSSetViewports(1, &viewport);

		constexpr float32 MIN_ROUGHNESS = 0.0001f;
		DirectX::XMStoreFloat4(&m_specularIrradianceCB.data.roughness, DirectX::XMVectorReplicate(std::fmax(mipmapLevel / static_cast<float32>(maxMipmapLevel - 1), MIN_ROUGHNESS)));
		m_specularIrradianceCB.applyChanges(1, graphics::MapTarget::PS);

		for (UINT i = 0; i < 6; ++i)
		{
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> d3d11RTV;
			RETURN_IF_FAILED(initializeSpecularIrradianceRTV(specularIrradianceCubemap, d3d11RTV, i, mipmapLevel));

			d3d::devcon->OMSetRenderTargets(1, d3d11RTV.GetAddressOf(), nullptr);

			FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
			d3d::devcon->ClearRenderTargetView(d3d11RTV.Get(), clearColor);

			d3d::devcon->PSSetShaderResources(0, 1, d3d11CubemapShaderResouceView.GetAddressOf());
			d3d::devcon->PSSetSamplers(0, 1, m_d3d11Sampler.GetAddressOf());

			DirectX::XMStoreFloat4x4(&m_worldToClipCB.data.worldToClip, m_worlToClipMatArr[i]);
			m_worldToClipCB.applyChanges(0, graphics::MapTarget::VS);

			m_unitCubeIndexBuffer.bind(0u);
			m_unitCubeVertexBuffer.bind(0u, 1u, 0u);

			m_specularIrradianceCubemapPipelineShaders.bind();

			d3d::devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

			d3d::devcon->DrawIndexed(36, 0, 0);
		}
	}

	DirectX::CaptureTexture(d3d::device, d3d::devcon, specularIrradianceCubemap.Get(), scratchImage);
	return S_OK;
}

HRESULT ReflectionCapture::generateSpecularReflectance2DTexture(
	const uint32 specularReflectance2DTextureSize,
	DirectX::ScratchImage& scratchImage)
{
	Microsoft::WRL::ComPtr<ID3D11Texture2D> specularReflectance2DTexture;
	RETURN_IF_FAILED(createRenderTexture2D(specularReflectance2DTexture, specularReflectance2DTextureSize));

	d3d::devcon->RSSetState(nullptr);
	d3d::devcon->OMSetDepthStencilState(nullptr, 0);

	D3D11_VIEWPORT viewport;
	RETURN_IF_FAILED(initializeViewport(viewport, specularReflectance2DTextureSize));
	d3d::devcon->RSSetViewports(1, &viewport);

	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> d3d11RTV;
	RETURN_IF_FAILED(d3d::device->CreateRenderTargetView(specularReflectance2DTexture.Get(), nullptr, d3d11RTV.GetAddressOf()));

	d3d::devcon->OMSetRenderTargets(1, d3d11RTV.GetAddressOf(), nullptr);

	FLOAT clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
	d3d::devcon->ClearRenderTargetView(d3d11RTV.Get(), clearColor);

	m_specularReflectance2DTexturePipelineShaders.bind();

	d3d::devcon->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	d3d::devcon->DrawIndexed(3, 0, 0);

	DirectX::CaptureTexture(d3d::device, d3d::devcon, specularReflectance2DTexture.Get(), scratchImage);
	return S_OK;
}

HRESULT ReflectionCapture::createRenderCubemapTexture(Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3dTexture2d, const uint32 size, const uint32 mipLevels)
{
	CD3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(CD3D11_TEXTURE2D_DESC));

	desc.Width = size;
	desc.Height = size;
	desc.MipLevels = mipLevels;
	desc.ArraySize = 6;
	desc.Format = m_d3d11TexFormat;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	
	
	HRESULT hr = d3d::device->CreateTexture2D(&desc, nullptr, d3dTexture2d.GetAddressOf());
	return hr;
}

HRESULT ReflectionCapture::createRenderTexture2D(Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3d11Texture2d, const uint32 size)
{
	CD3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(CD3D11_TEXTURE2D_DESC));

	desc.Width = size;
	desc.Height = size;
	desc.MipLevels = 0;
	desc.ArraySize = 1;
	desc.Format = m_d3d11TexFormat;
	desc.SampleDesc.Count = 1;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;


	HRESULT hr = d3d::device->CreateTexture2D(&desc, nullptr, d3d11Texture2d.GetAddressOf());
	return hr;
}

HRESULT ReflectionCapture::initializeDiffuseIrradianceRTVs(
	Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3d11CubemapTexture2D,
	std::array<Microsoft::WRL::ComPtr<ID3D11RenderTargetView>, 6>& d3d11RTVs)
{
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.MipSlice = 0;
	desc.Texture2DArray.ArraySize = 1;

	desc.Format = m_d3d11TexFormat;

	for (UINT i = 0; i < 6; ++i)
	{
		desc.Texture2DArray.FirstArraySlice = i;
		RETURN_IF_FAILED(
			d3d::device->CreateRenderTargetView(d3d11CubemapTexture2D.Get(), &desc, d3d11RTVs[i].GetAddressOf()));
	}

	return S_OK;
}

HRESULT ReflectionCapture::initializeSpecularIrradianceRTV(
	Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3d11CubemapTexture2D,
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& d3d11RTV,
	uint32 arraySlice,
	uint32 mipmapLevel)
{
	D3D11_RENDER_TARGET_VIEW_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	desc.Texture2DArray.ArraySize = 1;
	desc.Format = m_d3d11TexFormat;

	desc.Texture2DArray.MipSlice = mipmapLevel;
	desc.Texture2DArray.FirstArraySlice = arraySlice;

	HRESULT hr = d3d::device->CreateRenderTargetView(d3d11CubemapTexture2D.Get(), &desc, d3d11RTV.GetAddressOf());
	return hr;
}

HRESULT ReflectionCapture::initializeViewport(D3D11_VIEWPORT& viewport, const uint32 size)
{
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;
	viewport.Width = static_cast<float32>(size);
	viewport.Height = static_cast<float32>(size);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	return S_OK;
}

}