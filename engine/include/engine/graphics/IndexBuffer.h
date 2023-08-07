#pragma once

#include <engine/directx/D3D.h>

namespace graphics
{

class IndexBuffer
{
public:
	HRESULT initialize(const DWORD* data, UINT numIndices);
	void bind(UINT offset);
	UINT getBufferSize();
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	UINT bufferSize;
};

}