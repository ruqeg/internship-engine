#include <engine/graphics/IndexBuffer.h>

namespace graphics
{

HRESULT IndexBuffer::initialize(const DWORD* data, UINT numIndices)
{
	bufferSize = numIndices;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.ByteWidth = sizeof(DWORD) * numIndices;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexBufferData;
	indexBufferData.pSysMem = data;

	HRESULT hr = d3d::device->CreateBuffer(&desc, &indexBufferData, buffer.GetAddressOf());
	return hr;
}

void IndexBuffer::bind(UINT offset)
{
	d3d::devcon->IASetIndexBuffer(buffer.Get(), DXGI_FORMAT_R32_UINT, offset);
}

UINT IndexBuffer::getBufferSize()
{
	return bufferSize;
}

}