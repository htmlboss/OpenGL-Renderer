#include "Shader.h"
#include "ResourceLoader.h"

#include <iostream>

/***********************************************************************************/
Shader::Shader(const std::string_view shaderPath, const GLShaderType type) {

	const auto shaderCode = ResourceLoader::LoadTextFile(shaderPath);
	compileShader(shaderCode.c_str(), type);
}

/***********************************************************************************/
Shader::~Shader() {
}

/***********************************************************************************/
void Shader::compileShader(const GLchar* shaderSource, const GLShaderType shaderType) {

	m_shaderID = glCreateShader(shaderType);
	
	glShaderSource(m_shaderID, 1, &shaderSource, nullptr);
	glCompileShader(m_shaderID);
	glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &m_success);
	if (!m_success) {
		glGetShaderInfoLog(m_shaderID, m_infoLog.size(), nullptr, m_infoLog.data());
		std::cerr << "\nShader Error: " << m_shaderID << ": " << shaderType <<  " compilation failed:\n" << m_infoLog.data() << "--------" << std::endl;
		throw std::runtime_error(m_infoLog.data());
	}
}