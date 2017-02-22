#pragma once
// Utilities library

// Convert string to unsigned int so we can use switchs
constexpr size_t str2int(const char* str, const int h = 0) {
	return !str[h] ? 5381 : (str2int(str, h + 1) * 33) ^ str[h];
}