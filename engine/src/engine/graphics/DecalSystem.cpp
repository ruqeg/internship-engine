#pragma once

#include <engine/graphics/DecalSystem.h>

namespace graphics
{

DecalSystem& DecalSystem::getInstance()
{
	static DecalSystem instance;
	return instance;
}

void DecalSystem::initialize(uint32 w, uint32 h)
{
	D3D11_TEXTURE2D_DESC textureDesc{};

	textureDesc.Width = w; // Width of the texture
	textureDesc.Height = h; // Height of the texture
	textureDesc.MipLevels = 1; // Number of mipmap levels, in this case, just 1
	textureDesc.ArraySize = 1; // Number of textures in the array, in this case, just 1
	textureDesc.Format = DXGI_FORMAT_R16G16B16A16_SNORM; // Use DXGI_FORMAT_R8G8B8A8_UNORM for 8-bit per channel RGB texture
	textureDesc.SampleDesc.Count = 1; // Number of multisamples per pixel
	textureDesc.SampleDesc.Quality = 0; // Image quality level
	textureDesc.Usage = D3D11_USAGE_DEFAULT; // How the texture will be used
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE; // How the texture will be bound to the pipeline
	textureDesc.CPUAccessFlags = 0; // No CPU access required
	textureDesc.MiscFlags = 0; // No miscellaneous flags

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateTexture2D(&textureDesc, nullptr, m_normalCopyBuffer.ReleaseAndGetAddressOf()),
		L"Can't initialize Texture2D for DecalSystem!");

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateShaderResourceView(m_normalCopyBuffer.Get(), nullptr, m_normalCopyShaderResourceView.ReleaseAndGetAddressOf()),
		L"Can't initialize SRV for DecalSystem!");


	D3D11_TEXTURE2D_DESC depthStencilDesc{};
	depthStencilDesc.Width = w;
	depthStencilDesc.Height = h;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateTexture2D(&depthStencilDesc, NULL, m_depthCopyStencilBuffer.GetAddressOf()),
		L"Failed to create depth stencil buffer");

	D3D11_SHADER_RESOURCE_VIEW_DESC srv2Ddesc{};
	srv2Ddesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	srv2Ddesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srv2Ddesc.Texture2D.MipLevels = 1;

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateShaderResourceView(m_depthCopyStencilBuffer.Get(), &srv2Ddesc, m_depthCopyShaderResourceView.GetAddressOf()),
		L"Failed to create depth stencil view");

	std::vector<D3D11_INPUT_ELEMENT_DESC> shaderInputLayoutDesc =
	{
		// Data from the vertex buffer
		{ "POSITION", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 20, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 32, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "BITANGENT", 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 44, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA, 0 },

		// Data from the instance buffer
		{ "INSTANCEDECALTOWORLD1N", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCEDECALTOWORLD2N", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 16, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCEDECALTOWORLD3N", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 32, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCEDECALTOWORLD4N", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 48, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCEWORLDTODECAL1N", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 64, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCEWORLDTODECAL2N", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 80, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCEWORLDTODECAL3N", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 96, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCEWORLDTODECAL4N", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 112, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCECOLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 128, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1},
		{ "INSTANCEPARENTOBJECTID", 0, DXGI_FORMAT_R32_UINT, 1, 144, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_INSTANCE_DATA, 1}
	};

	m_pipelineShaders.vertexShader.initialize(utils::FileSystem::exeFolderPathW() + DECAL_SYSTEM_VERTEX_SHAER, shaderInputLayoutDesc);
	m_pipelineShaders.pixelShader.initialize(utils::FileSystem::exeFolderPathW() + DECAL_SYSTEM_PIXEL_SHAER);
}

void DecalSystem::setDecalInstances(const std::vector<DecalInstanceCPU>& decalInstances)
{
	m_decalInstances = decalInstances;
}

std::vector<DecalInstanceCPU>& DecalSystem::getDecalInstances()
{
	return m_decalInstances;
}

void DecalSystem::updateInstanceBuffer()
{
	std::vector<DecalInstanceGPU> instanceGPU(m_decalInstances.size());
	for (uint32 i = 0; i < m_decalInstances.size(); ++i)
	{
		instanceGPU[i] = m_decalInstances[i];
	}

	m_instanceBuffer.initialize(instanceGPU.data(), m_decalInstances.size());
}

void DecalSystem::render(
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& albedoRTV,
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& normalRTV,
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& roughMetallRTV,
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& emissionRTV,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& objectIDSRV,
	const Microsoft::WRL::ComPtr<ID3D11Texture2D>& depthBuffer,
	const Microsoft::WRL::ComPtr<ID3D11Texture2D>& normalBuffer,
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& depthStencilView)
{
	const auto& textureOpt = utils::TextureManager::getInstance().getTexture(DECAL_SYSTEM_TEXTURE);

	d3d::devcon->CopyResource(m_normalCopyBuffer.Get(), normalBuffer.Get());
	d3d::devcon->CopyResource(m_depthCopyStencilBuffer.Get(), depthBuffer.Get());

	std::array<ID3D11RenderTargetView*, 4> RTVs = { albedoRTV.Get(), normalRTV.Get(), roughMetallRTV.Get(), emissionRTV.Get() };
	d3d::devcon->OMSetRenderTargets(RTVs.size(), RTVs.data(), depthStencilView.Get());

	d3d::devcon->PSSetShaderResources(2u, 1, m_normalCopyShaderResourceView.GetAddressOf());
	d3d::devcon->PSSetShaderResources(4u, 1, m_depthCopyShaderResourceView.GetAddressOf());
	d3d::devcon->PSSetShaderResources(5u, 1, objectIDSRV.GetAddressOf());
	d3d::devcon->PSSetShaderResources(6u, 1, textureOpt.value()->shaderResourceView.GetAddressOf());
	
	const std::shared_ptr<Model>& model = utils::ModelManager::getInstance().getModel("../resources/models/cube.fbx").value();
	
	model->vertexBuffer.bind(0u, 1u, 0u);
	model->indexBuffer.bind(0u);
	
	m_instanceBuffer.bind(1u, 1u, 0u);
	
	m_pipelineShaders.bind();
	
	d3d::devcon->DrawIndexedInstanced(model->ranges.front().indexNum, m_decalInstances.size(), 0u, 0u, 0u);
}

DecalInstanceCPU::operator DecalInstanceGPU() const
{
	const MeshSystem::OpaqueInstanceGPU opaqueInstance = utils::SolidVector<MeshSystem::OpaqueInstanceGPU>::getInstance()[instanceID];

	const XMVECTOR offsetInWorldSpace = XMVector3Transform(offsetInModelSpace, XMMatrixTranspose(XMLoadFloat4x4(&opaqueInstance.modelToWorld)));
	const XMMATRIX translateMat = XMMatrixTranslationFromVector(offsetInWorldSpace);

	DecalInstanceGPU instanceGPU;
	DirectX::XMStoreFloat4(&instanceGPU.color, color);
	DirectX::XMStoreFloat4x4(&instanceGPU.decalToWorld, XMMatrixTranspose(scaledRotatedDecalBasis * translateMat));
	DirectX::XMStoreFloat4x4(&instanceGPU.worldToDecal, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&instanceGPU.decalToWorld)));
	instanceGPU.parentObjectID = opaqueInstance.objectID;
	return instanceGPU;
}

}