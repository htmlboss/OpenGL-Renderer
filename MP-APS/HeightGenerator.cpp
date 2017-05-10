#include "HeightGenerator.h"
#include "Utils/Utils.h"

#include <random>
#include <iostream>
#include <future>

/***********************************************************************************/
HeightGenerator::HeightGenerator() noexcept : m_amplitude(70.0f), m_roughness(0.3f), m_octaves(3) {
	std::random_device rd;
	std::mt19937_64 mt(rd());
	
	static const std::uniform_int_distribution<std::size_t> dist(0, std::numeric_limits<std::size_t>::max());
	m_seed = dist(mt);
	
	std::cout << "Terrain seed: " << m_seed << '\n';
}

/***********************************************************************************/
float HeightGenerator::GenerateHeight(const int x, const int z) const {
	// Larger denominators result in smoother terrain
	auto total = 0.0f;
	const float d = std::pow(2, m_octaves - 1);

	for (auto i = 0; i < m_octaves; ++i) {
		const auto freq = std::pow(2, i) / d;
		const auto amp = std::pow(m_roughness, i) * m_amplitude;

		total += getInterpolatedNoise(x * freq, z * freq) * amp;
	}

	return total;
}

/***********************************************************************************/
float HeightGenerator::getNoise(const int x, const int z) const noexcept {
	static const std::uniform_real_distribution<float> dis(-1.0f, std::nextafter(1.0f, std::numeric_limits<float>::max())); // Return value [-1, 1]
	std::mt19937_64 mt(x * 49632 + z * 325176 + m_seed);

	return dis(mt);
}

/***********************************************************************************/
float HeightGenerator::getSmoothNoise(const int x, const int z) const noexcept {
	const auto corners =	( getNoise(x-1, z-1)	// top left
							+ getNoise(x+1, z-1)	// top right
							+ getNoise(x-1, z+1)	// bottom left
							+ getNoise(x+1, z+1))	// bottom right
							/ 16.0f;
	const auto sides =	( getNoise(x-1, z)		// left
						+ getNoise(x+1, z)		// right
						+ getNoise(x, z-1)		// top
						+ getNoise(x, z+1))		// bottom
						/ 8.0f;
	const auto center = getNoise(x, z) / 4.0f;

	return corners + sides + center;
}

/***********************************************************************************/
float HeightGenerator::getInterpolatedNoise(const float x, const float z) const {

	const auto intX = static_cast<int>(x);
	const auto intZ = static_cast<int>(z);

	const auto fracX = x - intX;
	const auto fracZ = z - intZ;

	auto v1 = std::async(std::launch::async, &HeightGenerator::getSmoothNoise, this, intX, intZ);
	auto v2 = std::async(std::launch::async, &HeightGenerator::getSmoothNoise, this, intX + 1, intZ);
	auto v3 = std::async(std::launch::async, &HeightGenerator::getSmoothNoise, this, intX, intZ + 1);
	auto v4 = std::async(std::launch::async, &HeightGenerator::getSmoothNoise, this, intX + 1, intZ + 1);
	
	const auto i1 = Utils::cosInterpoloate(v1.get(), v2.get(), fracX);
	const auto i2 = Utils::cosInterpoloate(v3.get(), v4.get(), fracX);

	return Utils::cosInterpoloate(i1, i2, fracZ);
}
