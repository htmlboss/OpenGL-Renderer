#include "GLShader.h"
#include "../ResourceManager.h"

#include <iostream>

/***********************************************************************************/
GLShader::GLShader(const std::string_view path, const ShaderType type) {

	m_shaderID = glCreateShader(static_cast<int>(type));
	
	compile(ResourceManager::GetInstance().LoadTextFile(path).c_str());
}

/***********************************************************************************/
void GLShader::AttachShader(const GLuint Program) const {
	glAttachShader(Program, m_shaderID);
}

/***********************************************************************************/
void GLShader::DetachShader(const GLuint Program) const {
	glDetachShader(Program, m_shaderID);
}

/***********************************************************************************/
void GLShader::DeleteShader() const {
	glDeleteShader(m_shaderID);
}

/***********************************************************************************/
void GLShader::compile(const GLchar* shaderCode) {

	glShaderSource(m_shaderID, 1, &shaderCode, nullptr);
	glCompileShader(m_shaderID);

	glGetShaderiv(m_shaderID, GL_COMPILE_STATUS, &m_success);
	if (!m_success) {
		glGetShaderInfoLog(m_shaderID, m_infoLog.size(), nullptr, m_infoLog.data());
		std::cerr << "Shader Error: " << m_infoLog.data() << std::endl;
	}
}
