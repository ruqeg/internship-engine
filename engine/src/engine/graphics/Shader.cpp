#include <engine/graphics/Shader.h>

namespace graphics
{

Shader::Shader()
	:
	shaderPath(L"none")
{}

const std::wstring& Shader::getShaderPath() const
{
	return shaderPath;
}

}
