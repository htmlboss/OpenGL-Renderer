#include "GLTexture.h"

#include "../ResourceManager.h"

/***********************************************************************************/
GLTexture::GLTexture(const std::string_view ModelPath, const std::string_view TexturePath, const std::string_view samplerName, const WrapMode wrapMode) :
	m_samplerName(samplerName),
	m_texturePath(TexturePath) {

	m_fullPath = ModelPath;
	m_fullPath += std::string(TexturePath);

	glGenTextures(1, &m_texture);
	glBindTexture(GL_TEXTURE_2D, m_texture);
	// Set texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, static_cast<GLint>(wrapMode));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, static_cast<GLint>(wrapMode));
	// GLTexture filtering
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // linearly interpolates between the two closest mipmaps and samples the texture via linear interpolation.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // Mipmaps do not apply to magnification.
	if (GLAD_GL_EXT_texture_filter_anisotropic) {
		// Anisotropic filtering
		GLfloat aniso = 0.0f;
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso / 2.0f);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, 0.0f);
	}

	const auto texData = ResourceManager::GetInstance().GetTexture(m_fullPath, ResourceManager::ColorMode::RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, texData->width(), texData->height(), 0, GL_RGB, GL_UNSIGNED_BYTE, texData->data());

	glGenerateMipmap(GL_TEXTURE_2D);
}

/***********************************************************************************/
void GLTexture::Bind2D() {
	glBindTexture(GL_TEXTURE_2D, m_texture);
}
