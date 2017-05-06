#pragma once

#include <cstddef>

class HeightGenerator {
public:
	HeightGenerator();
	~HeightGenerator() = default;

	float GenerateHeight(const int x, const int z) const;

private:
	// Get noise at a particular point
	float getNoise(const int x, const int z) const;
	// Averages out noise over a 3x3 grid to remove sharpness
	/*
	* *---*---*
	* |   |   |
	* *---*---*
	* |   |   |
	* *---*---*
	*/
	float getSmoothNoise(const int x, const int z) const;
	// Create smooth curves ( ͡° ͜ʖ ͡°)
	float getInterpolatedNoise(const float x, const float z) const;

	const float m_amplitude, m_roughness;
	const std::size_t m_octaves;
	std::size_t m_seed;
};

