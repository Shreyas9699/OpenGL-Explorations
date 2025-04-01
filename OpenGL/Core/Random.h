#pragma once
#include <random>

class Random
{
public:
	static constexpr float PI = 3.14159265358979323846f;

	static void Init()
	{
		s_RandomEngine.seed(std::random_device()());
	}

	static float Float()
	{
		return (float)s_Distribution(s_RandomEngine) / (float)std::numeric_limits<unsigned int>::max();
	}

	static float Float(float min, float max)
	{
		return Random::Float() * (max - min) + min;
	}

	static int Int()
	{
		return (int)s_Distribution(s_RandomEngine);
	}

	static int Int(int min, int max)
	{
		return min + (s_Distribution(s_RandomEngine) % (max - min + 1));
	}

private:
	static std::mt19937 s_RandomEngine;
	static std::uniform_int_distribution<std::mt19937::result_type> s_Distribution;
};