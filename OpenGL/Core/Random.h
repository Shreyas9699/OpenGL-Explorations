#pragma once

#include <random>

class Random
{
public:
	static void Init()
	{
		s_RandomEngine.seed(std::random_device()());
	}

	static float Float()
	{
		return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<unsigned int>::max();
	}

	static float FloatRange(float min, float max) 
	{
		return Random::Float() * (max - min) + min;
	}

private:
	static std::mt19937 s_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};