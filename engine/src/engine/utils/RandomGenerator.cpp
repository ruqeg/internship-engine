#include <engine/utils/RandomGenerator.h>

namespace utils
{

RandomGenerator& RandomGenerator::getInstance()
{
	static RandomGenerator instance;
	return instance;
}

float32 RandomGenerator::randomFloat(float32 min, float32 max)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(min, max);
	return dist(gen);
}

}