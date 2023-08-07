#include <engine/graphics/HullShader.h>

namespace graphics
{

void HullShader::initialize(const std::wstring& shaderPath)
{
	this->shaderPath = shaderPath;

	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Failed to load hull shader: " + shaderPath);
		exit(-1);
	}

	hr = d3d::device->CreateHullShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, shader.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Failed to create hull shader: " + shaderPath);
		exit(-1);
	}
}

void HullShader::bind()
{
	d3d::devcon->HSSetShader(shader.Get(), NULL, 0);
}

}