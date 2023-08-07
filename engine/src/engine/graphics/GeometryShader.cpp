#include <engine/graphics/GeometryShader.h>

namespace graphics
{

void GeometryShader::initialize(const std::wstring& shaderPath)
{
	this->shaderPath = shaderPath;

	HRESULT hr = D3DReadFileToBlob(shaderPath.c_str(), shaderBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Failed to load geometry shader: " + shaderPath);
		exit(-1);
	}

	hr = d3d::device->CreateGeometryShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, shader.GetAddressOf());
	if (FAILED(hr))
	{
		utils::ErrorLogger::log(hr, L"Failed to create geometry shader: " + shaderPath);
		exit(-1);
	}
}

void GeometryShader::bind()
{
	d3d::devcon->GSSetShader(shader.Get(), NULL, 0);
}

}