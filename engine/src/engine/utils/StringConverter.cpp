#include <engine/utils/StringConverter.h>

namespace utils
{

std::wstring StringConverter::stringToWide(const std::string& str)
{
	return std::wstring(str.begin(), str.end());
}

std::string StringConverter::stringFromWide(const std::wstring& str)
{
	return std::string(str.begin(), str.end());
}

}