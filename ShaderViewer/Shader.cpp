#include "Shader.h"
#include "ResourceLoader.h"

#include <iostream>

/***********************************************************************************/
Shader::Shader(const char* shaderName) : m_linked(false), m_shaderName(shaderName) {
	m_program = glCreateProgram();
}

/***********************************************************************************/
Shader::~Shader() {
}

/***********************************************************************************/
void Shader::AddShader(const std::string& shaderSource, const ShaderType shaderType) {

	//std::cout << shaderSource << std::endl;
	
	glAttachShader(m_program, compileShader(ResourceLoader::LoadTextFile(shaderSource).c_str(), shaderType));
}

/***********************************************************************************/
void Shader::AddUniform(const std::string& uniform) {
	
	GLint uniformLocation = glGetUniformLocation(m_program, uniform.c_str());
	if (uniformLocation == -1) {
		std::cerr << "Error: " << uniform.c_str() << " uniform not found in shader: " << m_shaderName << std::endl;
		return;
	}
	m_uniformMap.try_emplace(uniform, uniformLocation);
}

/***********************************************************************************/
void Shader::Link() {
	// Link
	glLinkProgram(m_program);
	glGetProgramiv(m_program, GL_LINK_STATUS, &m_success);
	if (!m_success) {
		glGetProgramInfoLog(m_program, m_infoLog.size(), nullptr, m_infoLog.data());
		std::cerr << "Shader Error: " << m_shaderName << " failed to link:\n" << m_infoLog.data() << std::endl;
	}

	// Validate
	glValidateProgram(m_program);
	glGetProgramiv(m_program, GL_VALIDATE_STATUS, &m_success);
	if (!m_success) {
		glGetProgramInfoLog(m_program, m_infoLog.size(), nullptr, m_infoLog.data());
		std::cerr << "Shader Error: " << m_shaderName << " failed to validate:\n" << m_infoLog.data() << std::endl;
	}

	m_linked = true;
}

/***********************************************************************************/
void Shader::Bind() const {
	if (m_linked) {
		glUseProgram(m_program);
	}
	else {
		std::cerr <<  m_shaderName << " is not linked! Failed to bind.\n";
		throw std::runtime_error("");
	}
}

/***********************************************************************************/
GLuint Shader::compileShader(const GLchar* shaderSource, const ShaderType shaderType) {

	GLuint programID = glCreateShader(shaderType);
	
	glShaderSource(programID, 1, &shaderSource, nullptr);
	glCompileShader(programID);
	glGetShaderiv(programID, GL_COMPILE_STATUS, &m_success);
	if (!m_success) {
		glGetShaderInfoLog(programID, m_infoLog.size(), nullptr, m_infoLog.data());
		std::cerr << "\nShader Error: " << m_shaderName << ": " << shaderType <<  " compilation failed:\n" << m_infoLog.data() << "--------" << std::endl;
		throw std::runtime_error(m_infoLog.data());
	}
	return programID;
}