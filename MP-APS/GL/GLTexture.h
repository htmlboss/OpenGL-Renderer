#pragma once

#include <glad/glad.h>

#include <string>

class GLTexture {

public:
	enum class WrapMode {
		REPEAT = GL_REPEAT,
		MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
		EDGE_CLAMP = GL_CLAMP_TO_EDGE,
		BORDER_CLAMP = GL_CLAMP_TO_BORDER
	};

	GLTexture(const std::string_view ModelPath, std::string_view TexturePath, const std::string_view samplerName, const WrapMode wrapMode = WrapMode::REPEAT);

	bool operator==(const GLTexture& rhs) const { return m_fullPath == rhs.GetFullPath(); }

	void Bind2D();

	GLuint GetTexture() const noexcept { return m_texture; }
	std::string GetFullPath() const noexcept { return m_fullPath; }
	std::string GetRelativePath() const noexcept { return m_texturePath; }
	std::string GetSampler() const noexcept { return m_samplerName; }

private:
	GLuint m_texture;
	std::string m_samplerName, m_texturePath, m_fullPath;
};
