#pragma once

#include <engine/graphics/Shader.h>

namespace graphics
{

class ComputeShader : public Shader
{
public:
	void initialize(const std::wstring& shaderPath);
	void bind();
private:
	Microsoft::WRL::ComPtr<ID3D11ComputeShader> shader;
};

}