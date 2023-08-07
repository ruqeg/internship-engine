#pragma once

namespace d3d
{

#define RETURN_IF_FAILED(x)\
	if (HRESULT hr; FAILED(hr = x))\
		return hr;\

}