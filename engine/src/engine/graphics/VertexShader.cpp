#include <engine/graphics/VertexShader.h>

namespace graphics
{

void VertexShader::initialize(const std::wstring& shaderPath, const std::vector<D3D11_INPUT_ELEMENT_DESC>& shaderInputLayouts)
{
	this->shaderPath = shaderPath;

	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Failed to load vertex shader: " + shaderPath);
		exit(-1);
	}

	hr = d3d::device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, shader.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Failed to create vertex shader: " + shaderPath);
		exit(-1);
	}

	if (shaderInputLayouts.size() > 0)
	{
		hr = d3d::device->CreateInputLayout(
			shaderInputLayouts.data(),
			shaderInputLayouts.size(),
			shaderBuffer->GetBufferPointer(),
			shaderBuffer->GetBufferSize(),
			inputLayout.GetAddressOf());
		if (FAILED(hr))
		{
			utils::ErrorLogger::log(hr, L"Failed to create input layout for vertex shader: " + shaderPath);
			exit(-1);
		}
	}
}

void VertexShader::bind()
{
	d3d::devcon->IASetInputLayout(inputLayout.Get());
	d3d::devcon->VSSetShader(shader.Get(), NULL, 0);
}

}