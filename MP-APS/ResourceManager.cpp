#include "ResourceManager.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <cassert>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>

/***********************************************************************************/
std::string ResourceManager::LoadTextFile(const std::string_view path) const {
#ifdef _DEBUG
	std::cout << "Resource Manager: Loading text file: " << path << '\n';
#endif

	std::ifstream in(path.data(), std::ios::in);
	in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	if (!in) {
		std::cerr << "Resource Manager: File loading error: " + *path.data() << " " << errno << std::endl;
		std::abort();
	}

	return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

/***********************************************************************************/
unsigned int ResourceManager::LoadHDRI(const std::string_view path) const {
	//stbi_set_flip_vertically_on_load(true);
	
	// Dont flip HDR otherwise the probe will be upside down. We flip the y-coord in the
	// shader to correctly render the texture.
	int width, height, nrComp;
	auto* data = stbi_loadf(path.data(), &width, &height, &nrComp, 0);
	
	//stbi_set_flip_vertically_on_load(false);

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
unsigned int ResourceManager::LoadTexture(const std::string_view path, const bool useMipMaps) {

	// Check if texture is already loaded somewhere
	const auto val = m_textureCache.find(path.data());
	
	if (val != m_textureCache.end()) {
		// Found it
		return val->second;
	}

	// Create and cache a new texture
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path.data(), &width, &height, &nrComponents, 0);
	if (!data) {
		std::cerr << "Failed to load texture: " << path << std::endl;
		stbi_image_free(data);
		return 0;
	}

	GLenum format = 0;
	switch (nrComponents) {
	case 1:
		format = GL_RED;
		break;
	case 3:
		format = GL_RGB;
		break;
	case 4:
		format = GL_RGBA;
		break;
	}

	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
	if (useMipMaps) {
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	if (useMipMaps) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	stbi_image_free(data);

#ifdef _DEBUG
	std::cout << "Resource Manager: loaded texture: " << path << std::endl;
#endif

	return m_textureCache.try_emplace(path.data(), textureID).first->second;
}

/***********************************************************************************/
std::vector<char> ResourceManager::LoadBinaryFile(const std::string_view path) const {
	std::ifstream in(path.data(), std::ios::binary);
	in.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	if (!in) {
		std::cerr << "Resource Manager: Failed to load binary file: " << path << std::endl;
		std::abort();
	}

#ifdef _DEBUG
	std::cout << "Resource Manager: loaded binary file: " << path << std::endl;
#endif

	// Copy and return binary contents into vector
	return std::vector<char>(std::istreambuf_iterator<char>(in), (std::istreambuf_iterator<char>()));
}

/***********************************************************************************/
ModelPtr ResourceManager::GetModel(const std::string_view name, const std::string_view path) {

	// Check if model is already loaded.
	const auto val = m_modelCache.find(path.data());

	if (val == m_modelCache.end()) {
		// Load model, cache it, and return a shared_ptr to it.
		return m_modelCache.try_emplace(name.data(), std::make_shared<Model>(path, name)).first->second;
	}

	return val->second;
}

/***********************************************************************************/
ModelPtr ResourceManager::CacheModel(const std::string_view name, const Model model, const bool overwriteIfExists) {
	if (overwriteIfExists) {
		return m_modelCache.insert_or_assign(name.data(), std::make_shared<Model>(model)).first->second;
	}
	return m_modelCache.try_emplace(name.data(), std::make_shared<Model>(model)).first->second;
}
