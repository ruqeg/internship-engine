#include "RingBuffer.hpp"
namespace graphics
{

template<class T>
HRESULT RingBuffer<T>::initialize(UINT numVertices, const T* data)
{
	RETURN_IF_FAILED(initializeRangeBuffer());
	RETURN_IF_FAILED(initializeParticleBuffer(numVertices, data));
	return S_OK;
}

template<class T>
HRESULT RingBuffer<T>::initializeRangeBuffer()
{
	D3D11_BUFFER_DESC bufferDesc{};
	bufferDesc.ByteWidth = sizeof(RangeBufferCBS);
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.StructureByteStride = sizeof(RangeBufferCBS);
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

	RETURN_IF_FAILED(d3d::device->CreateBuffer(&bufferDesc, nullptr, m_rangeBuffer.ReleaseAndGetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_R32_UINT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0u;
	srvDesc.Buffer.NumElements = sizeof(RangeBufferCBS) / sizeof(UINT);

	RETURN_IF_FAILED(d3d::device->CreateShaderResourceView(m_rangeBuffer.Get(), &srvDesc, m_rangeBufferSRV.ReleaseAndGetAddressOf()));

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_R32_UINT;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0u;
	uavDesc.Buffer.NumElements = sizeof(RangeBufferCBS) / sizeof(UINT);
	uavDesc.Buffer.Flags = 0u;

	RETURN_IF_FAILED(d3d::device->CreateUnorderedAccessView(m_rangeBuffer.Get(), &uavDesc, m_rangeBufferUAV.ReleaseAndGetAddressOf()));
	
	return S_OK;
}

template<class T>
HRESULT RingBuffer<T>::initializeParticleBuffer(UINT numVertices, const T* data)
{
	D3D11_BUFFER_DESC bufferDesc{};

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(T) * numVertices;
	bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDesc.CPUAccessFlags = 0u;
	bufferDesc.StructureByteStride = sizeof(T);

	if (data != nullptr)
	{
		D3D11_SUBRESOURCE_DATA subData{};
		subData.pSysMem = data;
		RETURN_IF_FAILED(d3d::device->CreateBuffer(&bufferDesc, &subData, m_particleBuffer.ReleaseAndGetAddressOf()));
	}
	else
	{
		RETURN_IF_FAILED(d3d::device->CreateBuffer(&bufferDesc, nullptr, m_particleBuffer.ReleaseAndGetAddressOf()));
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0u;
	uavDesc.Buffer.NumElements = numVertices;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

	RETURN_IF_FAILED(d3d::device->CreateUnorderedAccessView(m_particleBuffer.Get(), &uavDesc, m_particleBufferUAV.ReleaseAndGetAddressOf()));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0u;
	srvDesc.Buffer.NumElements = numVertices;

	RETURN_IF_FAILED(d3d::device->CreateShaderResourceView(m_particleBuffer.Get(), &srvDesc, m_particleBufferSRV.ReleaseAndGetAddressOf()));
	
	return S_OK;
}

template<class T>
inline void RingBuffer<T>::updateRangeBuffer(const RangeBufferCBS& rangeBufferCBS)
{
	d3d::devcon->UpdateSubresource(m_rangeBuffer.Get(), 0, nullptr, &rangeBufferCBS, 0, 0);
}

template<class T>
Microsoft::WRL::ComPtr<ID3D11Buffer>& RingBuffer<T>::getParticelBuffer()
{
	return m_particleBuffer;
}

template<class T>
Microsoft::WRL::ComPtr<ID3D11Buffer>& RingBuffer<T>::getRangeBuffer()
{
	return m_rangeBuffer;
}

template<class T>
inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& RingBuffer<T>::getParticleBufferSRV()
{
	return m_particleBufferSRV;
}

template<class T>
inline Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& RingBuffer<T>::getParticleBufferUAV()
{
	return m_particleBufferUAV;
}

template<class T>
inline Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& RingBuffer<T>::getRangeBufferSRV()
{
	return m_rangeBufferSRV;
}

template<class T>
inline Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>& RingBuffer<T>::getRangeBufferUAV()
{
	return m_rangeBufferUAV;
}

}