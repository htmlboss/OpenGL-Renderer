/*
 * Utilities library:
 */
#pragma once

#include <cmath>
#include <future>
#include <random>
#include <glm/vec3.hpp>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef min
#undef min
#endif

#ifdef max
#undef max
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

/*
 * Random integer within [low, high] using mt19937_64 engine
 */
template<typename T>
constexpr auto randomInt(const T low, const T high) {
	std::random_device rd;
	std::mt19937_64 mt(rd());
	
	static const std::uniform_int_distribution<T> dist(low, std::nextafter(high, std::numeric_limits<T>::max()));

	return dist(mt);
}

/*
 * Random float/double within [low, high] using mt19937_64 engine
 */
template<typename T>
constexpr auto randomFloat(const T low, const T high) {
	std::random_device rd;
	std::mt19937_64 mt(rd());

	static const std::uniform_real_distribution<T> dist(low, std::nextafter(high, std::numeric_limits<T>::max()));

	return dist(mt);
}

/*
 * Wrapper to launch a function with std::launch::async, and returns a future.
 */
template<typename Func, typename... Params>
inline auto asyncFunc(Func&& f, Params&&... params) {
	return std::async(std::launch::async, std::forward<Func>(f), std::forward<Params>(params)...);
}

} // namespace Utils
