#pragma once

#include <unordered_map>
#include <memory>
#include <string_view>

#include "Texture.h"

class ResourceManager {
	ResourceManager() = default;
	~ResourceManager() = default;
public:

	static ResourceManager& GetInstance() {
		static ResourceManager instance;
		return instance;
	}

	ResourceManager(ResourceManager& other) = delete;
	ResourceManager& operator=(ResourceManager rhs) = delete;

	// Passed to stb_image as the number of components (channels) to load
	enum class ColorMode {
		GREY = 1,
		GREY_A,
		RGB,
		RGB_A
	};

	std::string LoadTextFile(const std::string_view path);

	TexturePtr GetTexture(const std::string_view path, const ColorMode mode);

	auto GetNumLoadedImages() const noexcept { return m_loadedTextures.size(); }

private:
	std::size_t m_currentTexID = 0;
	std::unordered_map<std::string, TexturePtr> m_loadedTextures;
};

