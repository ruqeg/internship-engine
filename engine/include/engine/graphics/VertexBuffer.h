#pragma once

#include <engine/directx/D3D.h>

namespace graphics
{

template<class T>
class VertexBuffer
{
public:
	HRESULT initialize(const T* data, UINT numVertices);
	void bind(UINT startSlot, UINT numBuffers, UINT offset);
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
	uint32 stride;
};

}

#include <engine/graphics/VertexBuffer.inl>