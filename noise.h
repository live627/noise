#pragma once

#include <cstdint>
#include <numeric>
#include <algorithm>
#include <random>

class PerlinNoise
{
public:
	PerlinNoise(
		const int width,
		const int height,
		const int octaves,
		const double freq,
		std::uint32_t seed = std::default_random_engine::default_seed)
		:
		width(width),
		height(height),
		size(height * width),
		octaves(octaves),
		fx(width / freq),
		fy(height / freq)
	{
		map = std::make_unique<double[]>(size);
		std::iota(std::begin(p), std::begin(p) + 256, 0);
		std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));
		std::iota(std::begin(p) + 256, std::end(p), 0);

		for (int index = 0; index < size; index++)
			map[index] = OctaveNoise((index / width) / fx, (index % width) / fy, 0.0);
	}
	auto GetMap() const { return map.get(); }

private:
	std::int32_t p[512];
	int width, height, size, octaves;
	double fx, fy;
	std::unique_ptr<double[]> map;

	static inline constexpr double Fade(const double t) noexcept
	{
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	static inline constexpr double Lerp(const double t, const double a, const double b) noexcept
	{
		return a + t * (b - a);
	}

	static inline constexpr double Grad(std::int32_t hash, const double x, const double y, const double z) noexcept
	{
		const std::int32_t h = hash & 15;
		const double u = h < 8 ? x : y;
		const double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
		return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
	}

	inline double Generate(double x, double y, double z) const
	{
		const std::int32_t X = static_cast<std::int32_t>(std::floor(x)) & 255;
		const std::int32_t Y = static_cast<std::int32_t>(std::floor(y)) & 255;
		const std::int32_t Z = static_cast<std::int32_t>(std::floor(z)) & 255;

		x -= std::floor(x);
		y -= std::floor(y);
		z -= std::floor(z);

		const double u = Fade(x);
		const double v = Fade(y);
		const double w = Fade(z);

		const std::int32_t A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
		const std::int32_t B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

		return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], x, y, z),
			Grad(p[BA], x - 1, y, z)),
			Lerp(u, Grad(p[AB], x, y - 1, z),
				Grad(p[BB], x - 1, y - 1, z))),
			Lerp(v, Lerp(u, Grad(p[AA + 1], x, y, z - 1),
				Grad(p[BA + 1], x - 1, y, z - 1)),
				Lerp(u, Grad(p[AB + 1], x, y - 1, z - 1),
					Grad(p[BB + 1], x - 1, y - 1, z - 1))));
	}

	double OctaveNoise(double x, double y, double z) const
	{
		double result = 0.0;
		double amp = 1.0;

		for (std::int32_t i = 0; i < octaves; ++i)
		{
			result += Generate(x, y, z) * amp;
			x *= 2.0;
			y *= 2.0;
			z *= 2.0;
			amp *= 0.5;
		}

		return result * 0.5 + 0.5;
	}
};