namespace graphics
{

template<class T>
HRESULT VertexBuffer<T>::initialize(const T* data, UINT numVertices)
{
	stride = sizeof(T);

	D3D11_BUFFER_DESC vertexBufferDecs;
	ZeroMemory(&vertexBufferDecs, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDecs.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
	vertexBufferDecs.ByteWidth = sizeof(T) * numVertices;
	vertexBufferDecs.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDecs.CPUAccessFlags = 0;
	vertexBufferDecs.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	vertexBufferData.pSysMem = data;

	return d3d::device->CreateBuffer(&vertexBufferDecs, &vertexBufferData, buffer.GetAddressOf());
}

template<class T>
void VertexBuffer<T>::bind(UINT startSlot, UINT numBuffers, UINT offset)
{
	d3d::devcon->IASetVertexBuffers(startSlot, numBuffers, buffer.GetAddressOf(), &stride, &offset);
}

}