#include "GLShaderProgram.h"

#include <iostream>

/***********************************************************************************/
GLShaderProgram::GLShaderProgram(const std::string_view programName, std::initializer_list<GLShader> shaders) : m_programName(programName) {

	m_programID = glCreateProgram();

	for (const auto& shader : shaders) { shader.AttachShader(m_programID); }

	try { linkAndValidate(); }
	catch (const std::runtime_error& err) { std::cerr << programName << " error: " << err.what() << '\n'; }

	// Cleanup
	for (auto& shader : shaders) {
		shader.DetachShader(m_programID);
		shader.DeleteShader();
	}
}

/***********************************************************************************/
void GLShaderProgram::Bind() const {
	glUseProgram(m_programID);
}

/***********************************************************************************/
void GLShaderProgram::AddUniform(const std::string_view uniform) {

	const GLint uniformLoc = glGetUniformLocation(m_programID, uniform.data());

	if (uniformLoc == -1) {
		std::cerr << "Error: " << uniform.data() << " uniform not found in shader: " << m_programName.c_str() << std::endl;
		return;
	}
	m_uniformMap.try_emplace(uniform, uniformLoc);
}

/***********************************************************************************/
void GLShaderProgram::AddUniforms(const std::initializer_list<std::string_view> uniforms) { for (const auto& it : uniforms) { AddUniform(it); } }

/***********************************************************************************/
void GLShaderProgram::DeleteProgram() const {
	if (m_programID != 0) {
		glDeleteProgram(m_programID);
	}
}

/***********************************************************************************/
void GLShaderProgram::linkAndValidate() {

	glLinkProgram(m_programID);
	glGetProgramiv(m_programID, GL_LINK_STATUS, &m_success);
	if (!m_success) {
		glGetProgramInfoLog(m_programID, m_infoLog.size(), nullptr, m_infoLog.data());
		throw std::runtime_error(m_infoLog.data());
	}

	glValidateProgram(m_programID);
	glGetProgramiv(m_programID, GL_VALIDATE_STATUS, &m_success);
	if (!m_success) {
		glGetProgramInfoLog(m_programID, m_infoLog.size(), nullptr, m_infoLog.data());
		throw std::runtime_error(m_infoLog.data());
	}
}
