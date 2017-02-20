#pragma once

#include <GL/glew.h>
#include <string_view>
#include "ResourceLoader.h"

class Texture {

public:

	// Texture wrap modes passed to OpenGL
	enum WrapMode {
		REPEAT = GL_REPEAT,
		MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
		EDGE_CLAMP = GL_CLAMP_TO_EDGE,
		BORDER_CLAMP = GL_CLAMP_TO_BORDER
	};

	enum TextureType {
		DIFFUSE = 0,
		HEIGHT,
		SPECULAR,
		AMBIENT // aka reflection map
	};

	//Texture();
	Texture(const std::string_view ModelPath, std::string_view TexturePath, const std::string_view samplerName, const WrapMode wrapMode, const ResourceLoader::ColorMode colorMode = ResourceLoader::ColorMode::RGB);
	~Texture();

	bool operator==(const Texture& rhs) const { return m_fullPath == rhs.GetFullPath(); }

	//Bind 2D texture
	void Bind2D();

	// Returns the global number of UNIQUE textures loaded
	static int GetLoadedTextures() { return m_numTextures; }

	GLuint GetTexture() const { return m_texture; }
	std::string GetFullPath() const { return m_fullPath; }
	std::string GetRelativePath() const { return m_texturePath; }
	std::string GetSampler() const { return m_samplerName; }

private:
	static int m_numTextures;
	GLuint m_texture;
	std::string m_samplerName;
	std::string m_texturePath, m_fullPath;
};

