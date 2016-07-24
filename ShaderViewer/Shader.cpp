#include "Shader.h"


/***********************************************************************************/
Shader::Shader(const GLchar* VertexShader, const GLchar* PixelShader) {

	// Vertex shader
	m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(m_vertexShader, 1, &VertexShader, NULL);
	glCompileShader(m_vertexShader);
	// Check for compile time errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(m_vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(m_vertexShader, 512, NULL, infoLog);
		std::cerr << "ERROR: vertex shader compilation failed:\n" << infoLog << std::endl;
	}

	// Fragment shader
	m_pixelShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(m_pixelShader, 1, &PixelShader, NULL);
	glCompileShader(m_pixelShader);
	// Check for compile time errors
	glGetShaderiv(m_pixelShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(m_pixelShader, 512, NULL, infoLog);
		std::cerr << "ERROR: fragment shader compilation failed:\n" << infoLog << std::endl;
	}

	// Link shaders
	m_shaderProgram = glCreateProgram();
	glAttachShader(m_shaderProgram, m_vertexShader);
	glAttachShader(m_shaderProgram, m_pixelShader);
	glLinkProgram(m_shaderProgram);
	// Check for linking errors
	glGetProgramiv(m_shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(m_shaderProgram, 512, NULL, infoLog);
		std::cerr << "ERROR: shaders failed to link:\n" << infoLog << std::endl;
	}

	//Clean up
	glDeleteShader(m_vertexShader);
	glDeleteShader(m_pixelShader);

}

/***********************************************************************************/
Shader::~Shader() {
}

/***********************************************************************************/
void Shader::Use() {
	glUseProgram(m_shaderProgram);
}
