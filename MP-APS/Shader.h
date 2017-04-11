#pragma once
#include "glad/glad.h"

#include <string_view>
#include <array>

class GLShaderProgram;

class Shader {
	friend class GLShaderProgram;
public:

	enum GLShaderType {
		PixelShader = GL_FRAGMENT_SHADER,
		VertexShader = GL_VERTEX_SHADER,
		GeometryShader = GL_GEOMETRY_SHADER
	};

	enum D3DShaderType {
		
	};

	Shader(const std::string_view shaderPath, const GLShaderType type);
	~Shader();

private:
	// OpenGL shader stuff
	void compileShader(const GLchar* shaderSource, const GLShaderType shaderType);

	GLuint m_shaderID;
	GLint m_success;
	std::array<GLchar, 1024> m_infoLog;
};

