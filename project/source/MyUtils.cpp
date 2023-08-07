#include "MyUtils.h"

MyUtils& MyUtils::getInstance()
{
	static MyUtils instance;
	return instance;
}

float64 MyUtils::secondsSinceStart()
{
	using namespace std::chrono;
	return duration_cast<duration<float64>>(high_resolution_clock::now() - programStartTime).count();
}

bool MyUtils::getClientUV(HWND hwnd, float32 x, float32 y, float32& u, float32& v)
{
	int32 width, height;
	if (!getClientSize(hwnd, width, height))
		return false;
	u = x / static_cast<float32>(width - 1);
	v = y / static_cast<float32>(height - 1);
	return true;
}

bool MyUtils::getClientSize(HWND hwnd, int32& width, int32& height)
{
	RECT rcClient;
	if (GetClientRect(hwnd, &rcClient))
	{
		width = rcClient.right - rcClient.left;
		height = rcClient.bottom - rcClient.top;
		return true;
	}
	return false;
}
