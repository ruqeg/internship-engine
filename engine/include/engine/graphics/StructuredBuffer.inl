namespace graphics
{

template<class T>
HRESULT StructuredBuffer<T>::initialize(const T* data, UINT numVertices)
{
	D3D11_BUFFER_DESC bufferDecs = {};

	bufferDecs.ByteWidth = sizeof(T) * numVertices;
	bufferDecs.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	bufferDecs.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	bufferDecs.StructureByteStride = sizeof(T);

	D3D11_SUBRESOURCE_DATA bufferData;
	ZeroMemory(&bufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	bufferData.pSysMem = data;

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateBuffer(&bufferDecs, &bufferData, m_buffer.ReleaseAndGetAddressOf()),
		L"Failed to create structured buffer");

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.ElementWidth = sizeof(T);
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;

	utils::ErrorHandler::exitOnFailed(
		d3d::device->CreateShaderResourceView(m_buffer.Get(), &srvDesc, m_shaderResourceView.ReleaseAndGetAddressOf()),
		L"Failed to create RTV for structured buffer");

	return S_OK;
}

template<class T>
void StructuredBuffer<T>::bind(UINT startSlot)
{
	d3d::devcon->PSSetShaderResources(startSlot, 1u, m_shaderResourceView.GetAddressOf());
}

}