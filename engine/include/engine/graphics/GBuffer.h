#pragma once

#include <engine/directx/D3D.h>

namespace graphics
{

struct SRV_RTV_Textuer2D
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture2d;
};

struct GBuffer
{
	SRV_RTV_Textuer2D albedoBuffer;
	SRV_RTV_Textuer2D roughnessMetallnessBuffer;
	SRV_RTV_Textuer2D normallBuffer;
	SRV_RTV_Textuer2D emissionBuffer;
	SRV_RTV_Textuer2D objectIDBuffer;
};

}