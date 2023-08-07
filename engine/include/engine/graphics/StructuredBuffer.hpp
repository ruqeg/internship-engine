#pragma once

#include <engine/directx/D3D.h>
#include <engine/utils/ErrorHandler.h>

namespace graphics
{

template<class T>
class StructuredBuffer
{
public:
	StructuredBuffer() = default;
	~StructuredBuffer() = default;
	StructuredBuffer(StructuredBuffer&) = delete;
	StructuredBuffer(StructuredBuffer&&) = delete;
	StructuredBuffer& operator=(StructuredBuffer&) = delete;
	StructuredBuffer& operator=(StructuredBuffer&&) = delete;
public:
	HRESULT initialize(const T* data, UINT numVertices);
	void bind(UINT startSlot);
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView;
};

}

#include <engine/graphics/StructuredBuffer.inl>