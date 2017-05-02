#pragma once

#include "glad/glad.h"
#include "../ResourceManager.h"

#include <string_view>

class GLTexture {

public:
	enum class WrapMode {
		REPEAT = GL_REPEAT,
		MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
		EDGE_CLAMP = GL_CLAMP_TO_EDGE,
		BORDER_CLAMP = GL_CLAMP_TO_BORDER
	};

	enum class TextureType {
		DIFFUSE = 0,
		HEIGHT,
		SPECULAR,
		AMBIENT // aka reflection map
	};

	GLTexture(const std::string_view ModelPath, std::string_view TexturePath, const std::string_view samplerName, const WrapMode wrapMode = WrapMode::REPEAT, const ResourceManager::ColorMode colorMode = ResourceManager::ColorMode::RGB);
	~GLTexture();

	bool operator==(const GLTexture& rhs) const { return m_fullPath == rhs.GetFullPath(); }

	//Bind 2D texture
	void Bind2D();

	GLuint GetTexture() const { return m_texture; }
	std::string GetFullPath() const { return m_fullPath; }
	std::string GetRelativePath() const { return m_texturePath; }
	std::string GetSampler() const { return m_samplerName; }

private:
	GLuint m_texture;
	std::string m_samplerName;
	std::string m_texturePath, m_fullPath;
};

