#include "Texture.h"

#include <iostream>
#include <log.h>

#define STB_IMAGE_IMPLEMENTATION
#define STBI_FAILURE_USERMSG
#include <stb_image.h>

int Texture::m_numTextures = 0; //Set initial textures to 0

/***********************************************************************************/
Texture::Texture(const std::string& TexturePath, const std::string& samplerName) : m_texturePath(TexturePath.c_str()), m_samplerName(samplerName) {
	
	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Set texture wrapping to GL_REPEAT
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Texture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // linearly interpolates between the two closest mipmaps and samples the texture via linear interpolation.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Mipmaps do not apply to magnification.

	//Load image
	int x, y;
	int n = 3; //Number of components to load (RGBA)
	unsigned char* data = stbi_load(TexturePath.c_str(), &x, &y, &n, STBI_rgb);

	if (data == NULL) {
		std::string error = "stb_image error (" + TexturePath + "): ";	 
		throw std::runtime_error(error + stbi_failure_reason());
		FILE_LOG(logERROR) << stbi_failure_reason();
	}

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	//Cleanup
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
