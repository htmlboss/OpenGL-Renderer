#pragma once

#include <string>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <GL/glew.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Shader.h"

class HUDText {
public:
	HUDText(Shader& shader, const std::string& FontPath, const GLint WindowWidth, const GLint WindowHeight);
	~HUDText();

	// Render Text to screen
	void RenderText(Shader& shader, const std::string& Text, GLfloat x, const GLfloat y, const GLfloat scale, const glm::vec3& color);

private:
	struct Character {
		GLuint TextureID;	// ID handle of the glyph texture
		glm::ivec2 Size;	// Size of glyph
		glm::ivec2 Bearing; // Offset from baseline to left/top of glyph
		GLuint Advance;		// Horizontal offset to advance to next glyph
	};

	std::map<GLchar, Character> m_characters;

	GLuint m_vao, m_vbo;
};

