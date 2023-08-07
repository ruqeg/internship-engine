#pragma once

#include <DirectXMath.h>
#include <engine/graphics/ConstantBuffer.h>
#include <engine/graphics/StructuredBuffer.hpp>
#include <engine/utils/ErrorHandler.h>

namespace graphics
{

struct RangeBufferCBS
{
	static constexpr uint32 ALIGNED_BYTE_OFFSET_FOR_ARGS = 4u * 3u;
	UINT number;
	UINT offset;
	UINT expired;
	UINT vertexCountPerInstance;
	UINT instanceCount;
	UINT startVertexLocation;
	UINT startInstanceLocation;
};

template<class T>
class RingBuffer
{
public:
	RingBuffer() = default;
	~RingBuffer() = default;
	RingBuffer(RingBuffer&) = delete;
	RingBuffer(RingBuffer&&) = delete;
	RingBuffer& operator=(RingBuffer&) = delete;
	RingBuffer& operator=(RingBuffer&&) = delete;
public:
	HRESULT initialize(UINT numVertices, const T* data = nullptr);
	void updateRangeBuffer(const RangeBufferCBS& rangeBufferCBS);
	Microsoft::WRL::ComPtr<ID3D11Buffer>& getParticelBuffer();
	Microsoft::WRL::ComPtr<ID3D11Buffer>& getRangeBuffer();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& getParticleBufferSRV();
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& getParticleBufferUAV();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& getRangeBufferSRV();
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& getRangeBufferUAV();
private:
	HRESULT initializeRangeBuffer();
	HRESULT initializeParticleBuffer(UINT numVertices, const T* data = nullptr);
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_particleBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_particleBufferSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_particleBufferUAV;
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_rangeBuffer;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_rangeBufferSRV;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_rangeBufferUAV;
};

} // namespace graphics

#include <engine/graphics/RingBuffer.inl>