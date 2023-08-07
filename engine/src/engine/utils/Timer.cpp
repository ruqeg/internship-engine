#include <engine/utils/Timer.h>

namespace utils
{

Timer::Timer()
{}

Timer::Timer(int32 newFps)
	: fps(newFps), invfps(1.0 / newFps)
{}

bool Timer::frameElapsed(float64& elapsed)
{
	auto currentFrameTime = high_resolution_clock::now();
	elapsed = duration_cast<duration<float64>>((currentFrameTime - lastLimitedFrameTime)).count();

	if (elapsed > invfps)
	{
		lastLimitedFrameTime = high_resolution_clock::now();
		return true;
	}
	else
	{
		std::this_thread::yield();
		return false;
	}
}

void Timer::setFps(int32 newFPS)
{
	fps = newFPS;
	invfps = 1.0 / newFPS;
}

}