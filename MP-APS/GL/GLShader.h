#pragma once
#include "glad/glad.h"

#include <string_view>
#include <array>

class GLShader {
public:

	enum class ShaderType {
		PixelShader = GL_FRAGMENT_SHADER,
		VertexShader = GL_VERTEX_SHADER,
		GeometryShader = GL_GEOMETRY_SHADER
	};

	GLShader(const std::string& shaderCode, const ShaderType type);
	~GLShader() = default;

	void AttachShader(const GLuint Program) const { glAttachShader(Program, m_shaderID); }
	void DetachShader(const GLuint Program) const { glDetachShader(Program, m_shaderID); }

	void DeleteShader() const { glDeleteShader(m_shaderID); }

private:
	void compile(const GLchar* shaderCode);

	GLuint m_shaderID;
	GLint m_success;
	std::array<GLchar, 1024> m_infoLog;
};

