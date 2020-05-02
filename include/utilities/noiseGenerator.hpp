#include <vector>
#include <numeric>
#include <random>
#include <algorithm>

#pragma once

class NoiseGenerator {
public:
	// Initialize with the reference values for the permutation vector
	NoiseGenerator() :
		primes({ { 995615039, 600173719, 701464987 },
				 { 831731269, 162318869, 136250887 },
				 { 174329291, 946737083, 245679977 },
				 { 362489573, 795918041, 350777237 },
				 { 457025711, 880830799, 909678923 },
				 { 787070341, 177340217, 593320781 },
				 { 405493717, 291031019, 391950901 },
				 { 458904767, 676625681, 424452397 },
				 { 531736441, 939683957, 810651871 },
				 { 997169939, 842027887, 423882827 } }),
		octaves(7),
		rd(),
		gen(rd()),
		dis(0, primes.size() - 1)
	{
		// Nothing to do.
	}

	template <typename T> T getNoise(T x, T y, double amplitude = 1, double persistence = 0.5)
	{
		double total = 0;
		double frequency = std::pow(2, octaves);
		for (size_t i = 0; i < octaves; ++i) {
			frequency /= 2;
			amplitude *= persistence;
			total += interpolatedNoise(static_cast<size_t>((dis(gen) + i) % (primes.size() - 1)),
									   x / frequency, y / frequency) *
					 amplitude;
		}
		return static_cast<T>(total / frequency);
	}

private:
	std::vector<std::vector<unsigned long> > primes;
	size_t octaves;
	std::random_device rd;
	std::mt19937 gen;
	std::uniform_int_distribution<size_t> dis;

	double noise(size_t i, size_t x, size_t y)
	{
		unsigned long n = x + y * 57;
		n = (n << 13) ^ n;
		unsigned long a = primes[i][0];
		unsigned long b = primes[i][1];
		unsigned long c = primes[i][2];
		unsigned long t = (n * (n * n * a + b) + c) & 0x7fffffff;
		return 1.0 - (double)(t) / 1073741824.0;
	}

	double smoothedNoise(size_t i, size_t x, size_t y)
	{
		double corners = (noise(i, x - 1, y - 1) + noise(i, x + 1, y - 1) + noise(i, x - 1, y + 1) +
						  noise(i, x + 1, y + 1)) /
						 16,
			   sides = (noise(i, x - 1, y) + noise(i, x + 1, y) + noise(i, x, y - 1) +
						noise(i, x, y + 1)) /
					   8,
			   center = noise(i, x, y) / 4;
		return corners + sides + center;
	}

	double interpolate(double a, double b, double x)
	{ // cosine interpolation
		double ft = x * 3.1415927, f = (1 - cos(ft)) * 0.5;
		return a * (1 - f) + b * f;
	}

	double interpolatedNoise(size_t i, double x, double y)
	{
		auto integer_X = static_cast<size_t>(x);
		auto integer_Y = static_cast<size_t>(y);
		auto fractional_X = x - static_cast<double>(integer_X);
		auto fractional_Y = y - static_cast<double>(integer_Y);

		double v1 = smoothedNoise(i, integer_X, integer_Y),
			   v2 = smoothedNoise(i, integer_X + 1, integer_Y),
			   v3 = smoothedNoise(i, integer_X, integer_Y + 1),
			   v4 = smoothedNoise(i, integer_X + 1, integer_Y + 1),
			   i1 = interpolate(v1, v2, fractional_X), i2 = interpolate(v3, v4, fractional_X);
		return interpolate(i1, i2, fractional_Y);
	}
};