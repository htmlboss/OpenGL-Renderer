#pragma once
#include <glad/glad.h>

#include <array>

class GLShader {

public:
	enum class ShaderType {
		Pixel = GL_FRAGMENT_SHADER,
		Vertex = GL_VERTEX_SHADER,
		Geometry = GL_GEOMETRY_SHADER,
		Compute = GL_COMPUTE_SHADER,
		TessControl = GL_TESS_CONTROL_SHADER,
		TessEval = GL_TESS_EVALUATION_SHADER
	};

	GLShader(const std::string_view path, const ShaderType type);

	void AttachShader(const GLuint Program) const;
	void DetachShader(const GLuint Program) const;
	void DeleteShader() const;

private:
	void compile(const GLchar* shaderCode);

	GLuint m_shaderID;
	GLint m_success;
	std::array<GLchar, 1024> m_infoLog;
};
