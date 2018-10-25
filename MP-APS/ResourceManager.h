#pragma once

#include "Model.h"

#include <unordered_map>
#include <optional>
#include <filesystem>

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
	std::string LoadTextFile(const std::filesystem::path& path) const;
	// Loads an HDR image and generates an OpenGL floating-point texture.
	unsigned int LoadHDRI(const std::string_view path) const;
	// Loads an image (if not cached) and generates an OpenGL texture.
	unsigned int LoadTexture(const std::string_view path, const bool useMipMaps = true, const bool useUnalignedUnpack = false);
	// Loads a binary file into a vector and returns it
	std::vector<char> LoadBinaryFile(const std::string_view path) const;
	
	ModelPtr GetModel(const std::string_view name, const std::string_view path);
	// Add a loaded model the the model cache
	ModelPtr CacheModel(const std::string_view name, const Model model, const bool overwriteIfExists = false);

	// Checks if a named material is cached. If so, we get an initialized optional. Otherwise we get an empty one.
	// It's up to the caller to check the result.
	std::optional<PBRMaterialPtr> GetMaterial(const std::string_view name) const;
	// Cache a material and get a handle back. Usually called if GetMaterial returns empty.
	PBRMaterialPtr CacheMaterial(const std::string_view name,
		const std::string_view albedoPath,
		const std::string_view aoPath,
		const std::string_view metallicPath,
		const std::string_view normalPath,
		const std::string_view roughnessPath,
		const std::string_view alphaMaskPath);


	// Removes a named model from cache.
	void UnloadModel(const std::string_view modelName);

	auto GetNumLoadedTextures() const noexcept { return m_textureCache.size(); }
	auto GetNumLoadedModels() const noexcept { return m_modelCache.size(); }
	auto GetNumMaterials() const noexcept { return m_materialCache.size(); }

private:
	std::unordered_map<std::string, ModelPtr> m_modelCache;
	std::unordered_map<std::string, unsigned int> m_textureCache;
	std::unordered_map<std::string, PBRMaterialPtr> m_materialCache;
};
