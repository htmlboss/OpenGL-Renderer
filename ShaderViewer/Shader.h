#pragma once
#include <GL\glew.h>

#include <iostream>
class Shader {
public:
	//Shader(const std::string& VertexShader, const std::string& PixelShader);
	Shader(const GLchar* VertexShader, const GLchar* PixelShader);
	Shader(const Shader& other) = delete;
	~Shader();

	void Use();

private:
	GLuint m_vertexShader, m_pixelShader, m_shaderProgram;
};

