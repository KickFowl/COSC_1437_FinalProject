#pragma once

#include <stdint.h>
#include <chrono>

class RNG
{
	uint32_t state;
	uint32_t xorshift32();

	uint32_t get_time_seed();
public:
	RNG();
	RNG(uint32_t seed);

	int getRandomRange(int min, int max);
};

