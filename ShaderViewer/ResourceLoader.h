#pragma once
#include <string_view>

class ResourceLoader {
public:
	// Passed to stb_image as the number of components (channels) to load
	enum ColorMode {
		GREY = 1,
		GREY_A , // Grey w/ alpha
		RGB,
		RGB_A // RGB w/ alpha
	};

	static std::string LoadTextFile(const std::string_view path);
	static unsigned char* LoadSTBImage(const char* filename, int* x, int* y, int* comp, const ColorMode mode);

private:
	ResourceLoader() = delete;
	~ResourceLoader() = delete;

	ResourceLoader(ResourceLoader& other) = delete;
	ResourceLoader(ResourceLoader&& other) = delete;
	ResourceLoader& operator=(ResourceLoader rhs) = delete;
	ResourceLoader& operator=(const ResourceLoader& rhs) = delete;
};

