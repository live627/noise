#include "noise.h"
#include <iostream>
#include <fstream>
#include <time.h>
#include <stdio.h>

using namespace std;

int main()
{
	std::uint32_t seed = 1, octaves = 8;
	const int mapXSize = 40, mapYSize = 450;
	double diff = 0, freq = 8;
	clock_t launch = clock();
	const PerlinNoise perlin(mapXSize, mapYSize, octaves, freq, seed);
	clock_t done = clock();
	diff = done - launch;
	auto map = perlin.GetMap();
	for (int y = 1; y <= mapXSize * mapYSize; y++)
	{
		std::clog << " " << static_cast<int>(map[y - 1] * 10);
		if (y % mapXSize == 0 && y != 0)
			std::clog << std::endl;
	}
	cout << diff << " ms" << std::endl;
	diff = 0;

	for (int i = 0; i < 121; i++)
	{
		clock_t launch = clock();

		const PerlinNoise perlin(mapXSize, mapYSize, octaves, freq, seed);

		clock_t done = clock();
		diff += done - launch;
	}
	cout << diff / 120 << " ms averaged 120 runs" << std::endl;
	cin.get();
	return 0;
}