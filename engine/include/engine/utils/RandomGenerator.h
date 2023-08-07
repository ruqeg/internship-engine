#pragma once

#include <stdlib.h>
#include <engine/types.h>
#include <random>

namespace utils
{

class RandomGenerator
{
public:
	static RandomGenerator& getInstance();
public:
	float32 randomFloat(float32 min, float32 max);
private:
	RandomGenerator() = default;
private:
};

}