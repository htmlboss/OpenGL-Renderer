#pragma once

#include <GL/glew.h>
#include <string>

class Texture {

public:
	// Passed to stb_image as the number of components (channels) to load
	enum ColorMode {
		GREY = 1,
		RGB = 3,
		RGB_A
	};

	// Texture wrap modes passed to OpenGL
	enum WrapMode {
		REPEAT = GL_REPEAT,
		MIRRORED_REPEAT = GL_MIRRORED_REPEAT,
		EDGE_CLAMP = GL_CLAMP_TO_EDGE,
		BORDER_CLAMP = GL_CLAMP_TO_BORDER
	};

	//Texture();
	Texture(const std::string& TexturePath, const std::string& samplerName, const WrapMode wrapMode, const ColorMode colorMode = ColorMode::RGB);
	~Texture();

	//Bind 2D texture
	void Bind2D();

	// Load image
	static unsigned char* LoadSTBImage(char const *filename, int *x, int *y, int *comp, int req_comp);

	// Returns the global number of UNIQUE textures loaded
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

