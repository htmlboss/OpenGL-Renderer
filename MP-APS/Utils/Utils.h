/*
 * Utilities library:
 * Random stuff I find online that can help
 */
#pragma once

namespace Utils {

/*
 *  Convert c_str to unsigned int so we can evaluate them in a switch statement
 */
constexpr size_t str2int(const char* str, const int h = 0) {
	return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}

} // namespace Utils
