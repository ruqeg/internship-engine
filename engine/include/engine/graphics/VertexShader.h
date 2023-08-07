#pragma once

#include <engine/graphics/Shader.h>

namespace graphics
{

class VertexShader : public Shader
{
public:
	void initialize(const std::wstring& shaderPath, const std::vector<D3D11_INPUT_ELEMENT_DESC>& shaderInputLayouts);
	void bind();
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
};

}