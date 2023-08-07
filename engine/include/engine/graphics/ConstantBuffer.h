#pragma once

#include <engine/directx/D3D.h>

namespace graphics
{

enum MapTarget
{
	VS = 0x00000001,
	PS = 0x00000010,
	HS = 0x00000100,
	DS = 0x00001000,
	GS = 0x00010000,
	CS = 0x00100000,
};

template<class T>
class ConstantBuffer
{
public:
	HRESULT initialize();
	HRESULT applyChanges(UINT startSlot, UINT mapTarget);
public: 
	T data;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
};

}

#include <engine/graphics/ConstantBuffer.inl>