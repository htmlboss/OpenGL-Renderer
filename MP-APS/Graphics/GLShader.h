#pragma once
#include <glad/glad.h>

#include <string>
#include <array>

class GLShader {
public:
	GLShader(const std::string_view path, const int type);
	GLShader(const std::string_view path, const std::string_view type);

	void AttachShader(const GLuint Program) const;
	void DetachShader(const GLuint Program) const;
	void DeleteShader() const;

private:
	// Implements the #include preprocessor directive for GLSL
	void scanForIncludes(std::string& shaderCode);
	// Replaces all occurances of a string in another string
	void replaceAll(std::string& str, const std::string& from, const std::string& to);
	// Compiles a shader file and checks for errors
	void compile(const GLchar* shaderCode);

	GLuint m_shaderID;

	// For compilation errors
	GLint m_success;
	std::array<GLchar, 1024> m_infoLog;
};
