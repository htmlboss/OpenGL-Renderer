/*
 * Utilities library:
 */
#pragma once

#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

namespace Utils {

/*
 *  Convert c_str to unsigned int so we can evaluate them in a switch statement
 */
constexpr std::size_t str2int(const char* str, const int h = 0) {
	return !str[h] ? 5381 : str2int(str, h + 1) * static_cast<unsigned long long>(33) ^ str[h];
}

/*
 * Linear interpolatation
 */
constexpr auto lerp(const float a, const float b, const float f) {
	return a + f*(b - a);
}

/*
 * Cosine interpolatation
 */
inline auto cosInterpoloate(const float a, const float b, const float blend) {
	const auto theta = blend * M_PI;
	const auto f = (1.0f - std::cos(theta)) * 0.5f;

	return a*(1.0f - f) + b*f;
}

} // namespace Utils
