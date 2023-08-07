#include <engine/utils/ErrorLogger.h>

namespace utils
{

void ErrorLogger::log(const std::string& message)
{
	const std::string errorMessgae = "Error: " + message;
	MessageBoxA(NULL, errorMessgae.c_str(), "Error", MB_ICONERROR);
}

void ErrorLogger::log(HRESULT hResult, const std::string& messgae)
{
	_com_error error(hResult);
	std::wstring errorMessage = L"Error: " + StringConverter::stringToWide(messgae) + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
}

void ErrorLogger::log(HRESULT hResult, const std::wstring& messgae)
{
	_com_error error(hResult);
	std::wstring errorMessage = L"Error: " + messgae + L"\n" + error.ErrorMessage();
	MessageBoxW(NULL, errorMessage.c_str(), L"Error", MB_ICONERROR);
}

}