#pragma once

#include <random>

class Noise
{
private:
	Noise() { mt.seed(rnd()); }

public: 
	static Noise& instance()
	{
		static Noise instance;
		return instance;
	}
	uint32_t get_rnd() { return static_cast<uint32_t> (mt()); }

	float random_range(float min, int max);
private:
	std::random_device rnd;
	std::mt19937 mt;
};

