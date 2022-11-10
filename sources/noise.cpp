#include "noise.h"

float Noise::random_range(float min, int max)
{
	_ASSERT_EXPR(max != 0, "division by zero");
	return static_cast<uint32_t> (mt()) % max + min;
}
