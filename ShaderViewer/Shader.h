#pragma once
#include <GL\glew.h>

#include <iostream>
#include <string>

class Shader {
public:
	Shader(const std::string& VertexShader, const std::string& PixelShader);
	Shader(const std::string& VertexShader, const std::string& PixelShader, const std::string& GeometryShader);

	Shader(const Shader& other) = delete;
	~Shader();

	void Use() const;
	GLint GetUniformLoc(const std::string& Uniform) const;

private:

	enum ShaderType {
		PixelShader = GL_FRAGMENT_SHADER,
		VertexShader = GL_VERTEX_SHADER,
		GeometryShader = GL_GEOMETRY_SHADER
	};

	// Generic function to open a shader file
	std::string readShader(const std::string& ShaderPath);

	void prepareShader(const GLchar* vertexShader, const GLchar* pixelShader);
	// Overload to compile geometry shader
	void prepareShader(const GLchar* vertexShader, const GLchar* pixelShader, const GLchar* geometryShader);
	// Generic function to compile any shader file
	void compileShader(const GLchar* shaderSource, const ShaderType shaderType, GLuint& programID);

	GLuint m_vertexShader, m_pixelShader, m_geometryShader, m_shaderProgram;
	//  To check for compile-time errors
	GLint m_success;
	GLchar m_infoLog[512];
};

