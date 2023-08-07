#pragma once

#include <engine/types.h>

#include <chrono>
#include <thread>

using namespace std::chrono;

namespace utils
{

class Timer
{
public:
	Timer();
	Timer(int32 newFps);

	bool frameElapsed(float64& elapsed);
	void setFps(int32 newFPS);
	inline int32 getFps() const;

private:
	steady_clock::time_point lastLimitedFrameTime = high_resolution_clock::now();

	int32 fps;
	float64 invfps;
};

#include <engine/utils/Timer.inl>

}
