#pragma once
#include <GL\glew.h>
#include <log.h>

#include <iostream>
#include <fstream>
#include <sstream>

class Shader {
public:
	Shader(const std::string& VertexShader, const std::string& PixelShader);
	Shader(const Shader& other) = delete;
	~Shader();

	void Use();

private:
	void prepareShader(const GLchar* vertexShader, const GLchar* pixelShader);

	GLuint m_vertexShader, m_pixelShader, m_shaderProgram;
};

