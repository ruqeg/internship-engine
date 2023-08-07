#pragma once

#include <engine/directx/D3D.h>

namespace graphics
{

class Shader
{
public:
	Shader();
	const std::wstring& getShaderPath() const;
protected:
	Microsoft::WRL::ComPtr<ID3D10Blob> shaderBuffer;
	std::wstring shaderPath;
};

}
