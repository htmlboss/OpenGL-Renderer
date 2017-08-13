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
		return std::string("Resource Manager: Bad string!");
	}

	return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

/***********************************************************************************/
unsigned int ResourceManager::LoadHDRI(const std::string_view path, const std::size_t resolution) {
	stbi_set_flip_vertically_on_load(true);
	
	int width, height, nrComp;
	auto* data = stbi_loadf(path.data(), &width, &height, &nrComp, 0);
	
	stbi_set_flip_vertically_on_load(false);

	if (!data) {
		std::cerr << "Resource Manager: Failed to load HDRI." << std::endl;
		//throw std::runtime_error(stbi_failure_reason());
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

	std::cout << "Resource Manager: Loaded HDR: " << path << '\n';

	return hdrTexture;
}

/***********************************************************************************/
TexturePtr ResourceManager::GetTexture(const std::string_view path, const ColorMode mode) {

	const auto val = m_loadedTextures.find(path.data());

	if (val == m_loadedTextures.end()) {
		// Image has not been loaded.
		int x, y, comp;
		auto data = stbi_load(path.data(), &x, &y, &comp, STBI_rgb);

		if (data == nullptr) {
			std::cerr << "Resource Manager: stb_image error (" << path << "): " << stbi_failure_reason() << '\n';
			throw std::runtime_error(stbi_failure_reason());
		}
		++m_currentTexID;
		// Add and return new loaded texture.
		return m_loadedTextures.emplace(path.data(), std::make_shared<Texture>(data, x, y, comp, m_currentTexID)).first->second;
	}

	return val->second;
}

/***********************************************************************************/
ModelPtr ResourceManager::GetModel(const std::string_view name, const std::string_view path) {

	const auto val = m_loadedModels.find(path.data());

	if (val == m_loadedModels.end()) {
		return m_loadedModels.emplace(name.data(), std::make_shared<Model>(path, name)).first->second;
	}

	return val->second;
}
