#pragma once

#include <GL\glew.h>
#include <string>

class Texture {
public:
	Texture(const std::string& TexturePath);
	~Texture();

	//Bind 2D texture
	void Bind2D();

	//Returns the global number of UNIQUE textures loaded
	static int GetLoadedTextures() { return m_numTextures; }

private:
	static int m_numTextures;
	GLuint m_texture;
};

