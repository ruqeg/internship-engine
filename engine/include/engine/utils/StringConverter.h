#pragma once

#include <string>

namespace utils
{

class StringConverter
{
public:
	static std::wstring stringToWide(const std::string& str);
	static std::string stringFromWide(const std::wstring& str);
};

}