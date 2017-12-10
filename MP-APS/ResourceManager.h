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
	enum class ColorMode : std::size_t {
		GREY = 1,
		GREY_A,
		RGB,
		RGB_A
	};

	// Load a text file and return as a string.
	std::string LoadTextFile(const std::string_view path);
	// Loads an HDR image and generates an OpenGL floating-point texture.
	unsigned int LoadHDRI(const std::string_view path) const;
	
	TexturePtr GetTexture(const std::string_view path, const ColorMode mode = ColorMode::RGB);
	ModelPtr GetModel(const std::string_view name, const std::string_view path);
	MaterialPtr GetMaterial(const std::string_view name);

	ModelPtr CacheModel(const std::string_view name, const Model& model, const bool overwriteIfExists = false);

	auto GetNumLoadedImages() const noexcept { return m_textureCache.size(); }
	auto GetNumLoadedModels() const noexcept { return m_modelCache.size(); }
	auto GetNumLoadedMaterials() const noexcept { return m_materialCache.size(); }

private:
	std::size_t m_currentTexID = 0;

	std::unordered_map<std::string, TexturePtr> m_textureCache;
	std::unordered_map<std::string, ModelPtr> m_modelCache;
	std::unordered_map<std::string, MaterialPtr> m_materialCache;
};
