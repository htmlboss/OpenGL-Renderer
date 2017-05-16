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
	
	std::ifstream in(path.data(), std::ios::in);
	if (!in) {
		std::cerr << "Failed to open: " + *path.data()  << " " << errno << '\n';
		return std::string("Bad string!");
	}

	return std::string(std::istreambuf_iterator<char>(in), std::istreambuf_iterator<char>());
}

/***********************************************************************************/
std::shared_ptr<Texture> ResourceManager::GetTexture(const std::string_view path, const ColorMode mode) {

	const auto val = m_loadedTextures.find(path.data());

	if (val == m_loadedTextures.end()) {
		// Image has not been loaded.
		int x, y, comp;
		auto data = stbi_load(path.data(), &x, &y, &comp, STBI_rgb);

		if (data == nullptr) {
			std::cerr << "stb_image error (" + std::string(path) + "): " + stbi_failure_reason() << '\n';
			throw std::runtime_error("");
		}
		++m_currentTexID;
		// Add and return new loaded texture.
		return m_loadedTextures.emplace(path.data(), std::make_shared<Texture>(data, x, y, comp, m_currentTexID)).first->second;
	}

	return val->second;
}