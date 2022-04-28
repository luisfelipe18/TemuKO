#include "stdafx.h"

static std::mt19937 s_randomNumberGenerator;
static std::recursive_mutex s_rngLock;
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
	Guard lock(s_rngLock);
	SeedRNG();
	if (min > max) std::swap(min, max);
	std::uniform_int_distribution<int32> dist(min, max);
	return dist(s_randomNumberGenerator);
}

uint64 RandUInt64()
{
	Guard lock(s_rngLock);
	SeedRNG();
	std::uniform_int_distribution<uint64> dist;
	return dist(s_randomNumberGenerator);
}

bool string_is_valid(const std::string &str2)
{
	std::string str = str2;
	STRTOLOWER(str);
	char a[36] = {'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4', '5', '6', '7', '8', '9', '0'};
	
	int size = (int) str.length();
	for(int i = 0; i < size; i++)
	{
		if(str.at(i) != a[0] 
		&& str.at(i) != a[1] 
		&& str.at(i) != a[2] 
		&& str.at(i) != a[3] 
		&& str.at(i) != a[4] 
		&& str.at(i) != a[5] 
		&& str.at(i) != a[6] 
		&& str.at(i) != a[7] 
		&& str.at(i) != a[8] 
		&& str.at(i) != a[9] 
		&& str.at(i) != a[10] 
		&& str.at(i) != a[11] 
		&& str.at(i) != a[12] 
		&& str.at(i) != a[13] 
		&& str.at(i) != a[14] 
		&& str.at(i) != a[15] 
		&& str.at(i) != a[16] 
		&& str.at(i) != a[17] 
		&& str.at(i) != a[18] 
		&& str.at(i) != a[19] 
		&& str.at(i) != a[20] 
		&& str.at(i) != a[21] 
		&& str.at(i) != a[22] 
		&& str.at(i) != a[23] 
		&& str.at(i) != a[24] 
		&& str.at(i) != a[25] 
		&& str.at(i) != a[25] 
		&& str.at(i) != a[26] 
		&& str.at(i) != a[27] 
		&& str.at(i) != a[28] 
		&& str.at(i) != a[29] 
		&& str.at(i) != a[30] 
		&& str.at(i) != a[31] 
		&& str.at(i) != a[32] 
		&& str.at(i) != a[33] 
		&& str.at(i) != a[34] 
		&& str.at(i) != a[35])
			return false;
	}

	return true;

}