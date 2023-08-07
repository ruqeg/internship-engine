#include <engine/graphics/ComputeShader.h>

namespace graphics
{

void ComputeShader::initialize(const std::wstring& shaderPath)
{
	this->shaderPath = shaderPath;

	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Failed to load compute shader: " + shaderPath);
		exit(-1);
	}

	hr = d3d::device->CreateComputeShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, shader.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Failed to create compute shader: " + shaderPath);
		exit(-1);
	}
}

void ComputeShader::bind()
{
	d3d::devcon->CSSetShader(shader.Get(), NULL, 0);
}

}