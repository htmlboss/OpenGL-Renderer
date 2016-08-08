#pragma once

#include <GL\glew.h>
#include <string>

class Texture {
public:
	Texture();
	Texture(const std::string& TexturePath, const std::string& samplerName);
	~Texture();

	//Bind 2D texture
	void Bind2D();

	//Returns the global number of UNIQUE textures loaded
	static int GetLoadedTextures() { return m_numTextures; }
	GLuint GetTexture() const { return m_texture; }
	std::string GetPath() const { return m_texturePath; }
	std::string GetSampler() const { return m_samplerName; }

private:
	static int m_numTextures;
	GLuint m_texture;
	std::string m_samplerName;
	const char* m_texturePath;
};

