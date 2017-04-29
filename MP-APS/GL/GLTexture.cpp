#include "GLTexture.h"

/***********************************************************************************/
GLTexture::GLTexture(const std::string_view ModelPath, const std::string_view TexturePath, const std::string_view samplerName, const WrapMode wrapMode, const ResourceManager::ColorMode colorMode) :
	m_texturePath(TexturePath), 
	m_samplerName(samplerName) {
	
	m_fullPath = ModelPath;
	m_fullPath += std::string(TexturePath);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	// GLTexture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // linearly interpolates between the two closest mipmaps and samples the texture via linear interpolation.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Mipmaps do not apply to magnification.
	// Anisotropic filtering
	GLfloat aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso / 2);

	const auto texData = ResourceManager::GetInstance().GetTexture(m_fullPath, colorMode);

	switch (colorMode) {
	case ResourceManager::ColorMode::GREY:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, texData->GetWidth(), texData->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, texData->GetData());
		break;

	case ResourceManager::ColorMode::RGB:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, texData->GetWidth(), texData->GetHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, texData->GetData());
		break;

	case ResourceManager::ColorMode::RGB_A:
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, texData->GetWidth(), texData->GetHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, texData->GetData());
		break;
	}
	glGenerateMipmap(GL_TEXTURE_2D);
}

/***********************************************************************************/
GLTexture::~GLTexture() {
}

/***********************************************************************************/
void GLTexture::Bind2D() {
	glBindTexture(GL_TEXTURE_2D, m_texture);
}