#include <engine/graphics/PixelShader.h>

namespace graphics
{

void PixelShader::initialize(const std::wstring& shaderPath)
{
	this->shaderPath = shaderPath;

	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Failed to load pixel shader: " + shaderPath);
		exit(-1);
	}

	hr = d3d::device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, shader.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Failed to create pixel shader: " + shaderPath);
		exit(-1);
	}
}

void PixelShader::bind()
{
	d3d::devcon->PSSetShader(shader.Get(), NULL, 0);
}

}