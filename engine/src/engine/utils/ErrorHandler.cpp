#include <engine/utils/ErrorHandler.h>

namespace utils
{

void ErrorHandler::exitOnFailed(HRESULT hr, const std::wstring& msg)
{
	if (SUCCEEDED(hr))
		return;

	utils::ErrorLogger::log(hr, msg);
	exit(-1);
}

}