#pragma once

#include <engine/graphics/Shader.h>

namespace graphics
{

class GeometryShader : public Shader
{
public:
	void initialize(const std::wstring& shaderPath);
	void bind();
private:
	Microsoft::WRL::ComPtr<ID3D11GeometryShader> shader;
};

}