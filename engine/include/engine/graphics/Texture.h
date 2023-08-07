#pragma once

#include <engine/directx/D3D.h>

namespace graphics
{

struct Texture
{
	Microsoft::WRL::ComPtr<ID3D11Resource> resource;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
};

}