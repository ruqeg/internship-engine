#include <engine/utils/FileSystem.h>

namespace utils
{

std::wstring FileSystem::exeFolderPathW()
{
	std::wstring pasth;
#pragma region DetermineShaderPath
#ifdef _DEBUG
#ifdef _WIN64 //x64
	pasth = L"x64\\Debug\\";
#else   //x86 (WIN32)
	pasth = L"Debug\\";
#endif
#else //Release Mode
#ifdef _WIN64 //x64
	pasth = L"x64\\Release\\";
#else   //x86 (WIN32)
	pasth = L"Release\\";
#endif
#endif
	return pasth;
}

}