namespace graphics
{

template<class T>
HRESULT ConstantBuffer<T>::initialize()
{
	D3D11_BUFFER_DESC desc;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0u;
	desc.ByteWidth = static_cast<UINT>(sizeof(T) + static_cast<UINT>(16u - static_cast<INT>(sizeof(T) % 16u)));
	desc.StructureByteStride = 0u;

	return d3d::device->CreateBuffer(&desc, 0, buffer.GetAddressOf());
}

template<class T>
HRESULT ConstantBuffer<T>::applyChanges(UINT startSlot, UINT mapTarget)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	HRESULT hr = d3d::devcon->Map(buffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if (SUCCEEDED(hr))
	{
		CopyMemory(mappedResource.pData, &data, sizeof(T));
		d3d::devcon->Unmap(buffer.Get(), 0);
		if (mapTarget & MapTarget::VS)
		{
			d3d::devcon->VSSetConstantBuffers(startSlot, 1, buffer.GetAddressOf());
		}
		if (mapTarget & MapTarget::PS)
		{
			d3d::devcon->PSSetConstantBuffers(startSlot, 1, buffer.GetAddressOf());
		}
		if (mapTarget & MapTarget::HS)
		{
			d3d::devcon->HSSetConstantBuffers(startSlot, 1, buffer.GetAddressOf());
		}
		if (mapTarget & MapTarget::DS)
		{
			d3d::devcon->DSSetConstantBuffers(startSlot, 1, buffer.GetAddressOf());
		}
		if (mapTarget & MapTarget::GS)
		{
			d3d::devcon->GSSetConstantBuffers(startSlot, 1, buffer.GetAddressOf());
		}
		if (mapTarget & MapTarget::CS)
		{
			d3d::devcon->CSSetConstantBuffers(startSlot, 1, buffer.GetAddressOf());
		}
	}
	return hr;
}

}