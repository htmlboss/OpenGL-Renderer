#pragma once

#include "Model.h"

#include <unordered_map>

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

	void ReleaseAllResources();

	// Load a text file and return as a string.
	std::string LoadTextFile(const std::string_view path) const;
	// Loads an HDR image and generates an OpenGL floating-point texture.
	unsigned int LoadHDRI(const std::string_view path) const;
	// Loads an image (if not cached) and generates an OpenGL texture.
	unsigned int LoadTexture(const std::string_view path, const bool useMipMaps = true);
	// Loads a binary file into a vector and returns it
	std::vector<char> LoadBinaryFile(const std::string_view path) const;
	
	ModelPtr GetModel(const std::string_view name, const std::string_view path);
	// Add a loaded model the the model cache
	ModelPtr CacheModel(const std::string_view name, const Model model, const bool overwriteIfExists = false);

	auto GetNumLoadedTextures() const noexcept { return m_textureCache.size(); }
	auto GetNumLoadedModels() const noexcept { return m_modelCache.size(); }

private:
	std::unordered_map<std::string, ModelPtr> m_modelCache;
	std::unordered_map<std::string, unsigned int> m_textureCache;
};
