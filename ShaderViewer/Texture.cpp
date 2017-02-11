#include "Texture.h"

#include <iostream>
#include <log.h>

int Texture::m_numTextures = 0; //Set initial textures to 0
/***********************************************************************************/
Texture::Texture(const std::string& TexturePath, const std::string& samplerName, const WrapMode wrapMode, const ResourceLoader::ColorMode colorMode) : 
	m_texturePath(TexturePath.c_str()), 
	m_samplerName(samplerName) {
	
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrapMode);
	// Texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // linearly interpolates between the two closest mipmaps and samples the texture via linear interpolation.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Mipmaps do not apply to magnification.
	// Anisotropic filtering
	GLfloat aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso / 2);

	// Load image
	int x, y;
	int n = 0;
	const auto data = ResourceLoader::LoadSTBImage(TexturePath.c_str(), &x, &y, &n, colorMode);
	
	if (!data) {
		throw std::runtime_error("");
	}

	switch (colorMode) {
	case ResourceLoader::GREY:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		break;

	case ResourceLoader::ColorMode::RGB:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		break;

	case ResourceLoader::ColorMode::RGB_A:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		break;
	}
	glGenerateMipmap(GL_TEXTURE_2D);

	// Cleanup
	glBindTexture(GL_TEXTURE_2D, 0);
	delete[] data;

	++m_numTextures;
}

/***********************************************************************************/
Texture::~Texture() {
}

/***********************************************************************************/
void Texture::Bind2D() {
	glBindTexture(GL_TEXTURE_2D, m_texture);
}