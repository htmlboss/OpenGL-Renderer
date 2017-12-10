#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>

/***********************************************************************************/
std::string ResourceManager::LoadTextFile(const std::string_view path) {
#ifdef _DEBUG
	std::cout << "Resource Manager: Loading text file: " << path << '\n';
#endif

	std::ifstream in(path.data(), std::ios::in);
	in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	if (!in) {
		std::cerr << "Resource Manager: File loading error: " + *path.data() << " " << errno << '\n';
		std::abort();
	}

	return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

/***********************************************************************************/
unsigned int ResourceManager::LoadHDRI(const std::string_view path) const {
	stbi_set_flip_vertically_on_load(true);
	
	int width, height, nrComp;
	auto* data = stbi_loadf(path.data(), &width, &height, &nrComp, 0);
	
	stbi_set_flip_vertically_on_load(false);

	if (!data) {
		std::cerr << "Resource Manager: Failed to load HDRI." << std::endl;
		std::abort();
	}

	unsigned int hdrTexture;
	glGenTextures(1, &hdrTexture);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

#ifdef _DEBUG
	std::cout << "Resource Manager: Loaded HDR: " << path << std::endl;
#endif

	return hdrTexture;
}

/***********************************************************************************/
TexturePtr ResourceManager::GetTexture(const std::string_view path, const ColorMode mode) {

	const auto val = m_textureCache.find(path.data());

	if (val == m_textureCache.end()) {
		// Image has not been loaded.
		int x, y, comp;
		auto data = stbi_load(path.data(), &x, &y, &comp, static_cast<int>(mode));

		if (data == nullptr) {
			std::cerr << "Resource Manager: stb_image error (" << path << "): " << stbi_failure_reason() << std::endl;
			std::abort();
		}
		++m_currentTexID;
		// Add and return new loaded texture.
		return m_textureCache.try_emplace(path.data(), std::make_shared<Texture>(data, x, y, comp, m_currentTexID)).first->second;
	}

	return val->second;
}

/***********************************************************************************/
ModelPtr ResourceManager::GetModel(const std::string_view name, const std::string_view path) {

	const auto val = m_modelCache.find(path.data());

	if (val == m_modelCache.end()) {
		// Load model, cache it, and return a shared_ptr to it.
		return m_modelCache.try_emplace(name.data(), std::make_shared<Model>(path, name)).first->second;
	}

	return val->second;
}

/***********************************************************************************/
MaterialPtr ResourceManager::GetMaterial(const std::string_view name) {
	const auto val = m_materialCache.find(name.data());

	if (val == m_materialCache.end()) {
		// Create empty material and return shared_ptr
#ifdef _DEBUG
		std::cout << "Failed to find material: " << name << std::endl;
#endif
		return m_materialCache.try_emplace(name.data(), std::make_shared<Material>()).first->second;
	}

	return val->second;
}

/***********************************************************************************/
ModelPtr ResourceManager::CacheModel(const std::string_view name, const Model& model, const bool overwriteIfExists) {
	if (overwriteIfExists) {
		return m_modelCache.insert_or_assign(name.data(), std::make_shared<Model>(model)).first->second;
	}
	return m_modelCache.try_emplace(name.data(), std::make_shared<Model>(model)).first->second;
}
