#include "Texture.h"

#include <iostream>
#include <log.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>

int Texture::m_numTextures = 0; //Set initial textures to 0
/***********************************************************************************/
Texture::Texture(const std::string& TexturePath, const std::string& samplerName, const WrapMode wrapMode, const ColorMode colorMode) : 
	m_texturePath(TexturePath.c_str()), 
	m_samplerName(samplerName) {
	
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);	// Set texture wrapping to GL_REPEAT
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
	int n = colorMode; // Number of components to load (RGBA)
	unsigned char* data;
	try {
		data = LoadSTBImage(TexturePath.c_str(), &x, &y, &n, colorMode);
	}
	catch (const std::runtime_error& e) {
		std::cerr << e.what() << '\n';
	}

	switch (colorMode) {
	case ColorMode::GREY:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		break;

	case ColorMode::RGB:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		break;

	case ColorMode::RGB_A:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		break;
	}
	glGenerateMipmap(GL_TEXTURE_2D);

	// Cleanup
	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(data);

	++m_numTextures;
}

/***********************************************************************************/
Texture::~Texture() {
}

/***********************************************************************************/
void Texture::Bind2D() {
	glBindTexture(GL_TEXTURE_2D, m_texture);
}

/***********************************************************************************/
unsigned char* Texture::LoadSTBImage(const char* filename, int* x, int* y, int* comp, int req_comp) {
	
	unsigned char* data = stbi_load(filename, x, y, comp, req_comp);
	
	if (data == NULL) {
		std::string error = "stb_image error (" + std::string(filename) + "): " + stbi_failure_reason();
		FILE_LOG(logERROR) << error;
		throw std::runtime_error(error);
	}
	return data;
}
