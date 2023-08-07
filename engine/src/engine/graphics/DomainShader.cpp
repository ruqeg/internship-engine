#include <engine/graphics/DomainShader.h>

namespace graphics
{

void DomainShader::initialize(const std::wstring& shaderPath)
{
	this->shaderPath = shaderPath;

	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Failed to load domain shader: " + shaderPath);
		exit(-1);
	}

	hr = d3d::device->CreateDomainShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, shader.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Failed to create domain shader: " + shaderPath);
		exit(-1);
	}
}

void DomainShader::bind()
{
	d3d::devcon->DSSetShader(shader.Get(), NULL, 0);
}

}