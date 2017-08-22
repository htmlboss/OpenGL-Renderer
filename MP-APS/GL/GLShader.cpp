#include "GLShader.h"
#include "../ResourceManager.h"

#include <iostream>

/***********************************************************************************/
const std::unordered_map<std::string_view, int> GLShader::m_shaderTypes
{
	{"vertex", GL_VERTEX_SHADER },
	{"pixel", GL_FRAGMENT_SHADER },
	{"fragment", GL_FRAGMENT_SHADER },
	{"geometry", GL_GEOMETRY_SHADER },
	{"compute", GL_COMPUTE_SHADER },
	{"tessctrl", GL_TESS_CONTROL_SHADER },
	{"tesseval", GL_TESS_EVALUATION_SHADER }
};

/***********************************************************************************/
GLShader::GLShader(const std::string_view path, const int type) {

	m_shaderID = glCreateShader(type);

	compile(ResourceManager::GetInstance().LoadTextFile(path).c_str());
}

/***********************************************************************************/
GLShader::GLShader(const std::string_view path, const std::string_view type) {

	m_shaderID = glCreateShader(m_shaderTypes.at(type));
	
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
