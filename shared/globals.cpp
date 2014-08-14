#include "stdafx.h"

static std::mt19937 s_randomNumberGenerator;
<<<<<<< HEAD
static FastMutex s_rngLock;
=======
static std::recursive_mutex s_rngLock;
>>>>>>> koserver2
static bool s_rngSeeded = false;

INLINE void SeedRNG()
{
	if (!s_rngSeeded)
	{
		s_randomNumberGenerator.seed((unsigned long)getMSTime());
		s_rngSeeded = true;
	}
}

int32 myrand(int32 min, int32 max)
{
<<<<<<< HEAD
	FastGuard lock(s_rngLock);
=======
	Guard lock(s_rngLock);
>>>>>>> koserver2
	SeedRNG();
	if (min > max) std::swap(min, max);
	std::uniform_int_distribution<int32> dist(min, max);
	return dist(s_randomNumberGenerator);
}

uint64 RandUInt64()
{
<<<<<<< HEAD
	FastGuard lock(s_rngLock);
=======
	Guard lock(s_rngLock);
>>>>>>> koserver2
	SeedRNG();
	std::uniform_int_distribution<uint64> dist;
	return dist(s_randomNumberGenerator);
}