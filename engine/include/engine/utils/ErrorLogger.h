#pragma once

#include <engine/utils/StringConverter.h>

#include <Windows.h>
#include <comdef.h>

namespace utils
{

class ErrorLogger
{
public:
	static void log(const std::string& message);
	static void log(HRESULT hResult, const std::string& messgae);
	static void log(HRESULT hResult, const std::wstring& messgae);
};

}