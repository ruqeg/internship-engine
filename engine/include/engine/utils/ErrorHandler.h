#pragma once

#include <engine/utils/ErrorLogger.h>

namespace utils
{

class ErrorHandler
{
public:
	static void exitOnFailed(HRESULT hr, const std::wstring& msg);
};

}

#define RETURN_IF_FAILED(hResultFun)\
	if (const HRESULT hr = hResultFun; FAILED(hr))\
		return hr;