#pragma once

#include "Texture.h"
#include "Model.h"

#include <unordered_map>
#include <string_view>

class ResourceManager {
	ResourceManager() = default;
	~ResourceManager() = default;
public:

	static auto& GetInstance() {
		static ResourceManager instance;
		return instance;
	}

	ResourceManager(const ResourceManager&) = delete;
	ResourceManager& operator=(const ResourceManager&) = delete;

	// Passed to stb_image as the number of components (channels) to load
	enum class ColorMode {
		GREY = 1,
		GREY_A,
		RGB,
		RGB_A
	};

	std::string LoadTextFile(const std::string_view path);

	// Loads an HDR image and generates an OpenGL floating-point texture.
	unsigned int LoadHDRI(const std::string_view path, const std::size_t resolution);
	TexturePtr GetTexture(const std::string_view path, const ColorMode mode);
	ModelPtr GetModel(const std::string_view name, const std::string_view path);

	auto GetNumLoadedImages() const noexcept { return m_loadedTextures.size(); }
	auto GetNumLoadedModels() const noexcept { return m_loadedModels.size(); }

private:
	size_t m_currentTexID = 0;

	std::unordered_map<std::string, TexturePtr> m_loadedTextures;
	std::unordered_map<std::string, ModelPtr> m_loadedModels;
};
