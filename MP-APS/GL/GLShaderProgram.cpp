#include "GLShaderProgram.h"
#include "GLShader.h"

#include <iostream>
#include <string_view>

/***********************************************************************************/
GLShaderProgram::GLShaderProgram(const std::string_view programName, std::initializer_list<GLShader> shaders) : m_programName(programName) {

	m_programID = glCreateProgram();

	for (const auto& shader : shaders) {
		shader.AttachShader(m_programID);
	}

	try {
		linkAndValidate();
	}
	catch (const std::runtime_error& err) {
		std::cerr << programName << " error: " << err.what() << '\n';
	}

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
void GLShaderProgram::AddUniforms(const std::initializer_list<std::string_view> uniforms) { 
	
	for (const auto& uniform : uniforms) {
		const auto uniformLoc = glGetUniformLocation(m_programID, uniform.data());

		if (uniformLoc == -1) {
			std::cerr << "Shader Error: " << uniform.data() << " uniform not found in shader: " << m_programName.c_str() << '\n';
		}

		m_uniforms.try_emplace(uniform, uniformLoc);
	}
}

/***********************************************************************************/
void GLShaderProgram::DeleteProgram() const {
	if (m_programID != 0) {
		glDeleteProgram(m_programID);
	}
}

/***********************************************************************************/
void GLShaderProgram::SetUniformi(const std::string_view uniformName, const GLint value) const {
	glUniform1i(m_uniforms.at(uniformName.data()), value);
}

/***********************************************************************************/
void GLShaderProgram::SetUniformf(const std::string_view uniformName, const GLfloat value) const {
	glUniform1f(m_uniforms.at(uniformName.data()), value);
}

/***********************************************************************************/
void GLShaderProgram::SetUniform(const std::string_view uniformName, const glm::vec3& value) const {
	glUniform3f(m_uniforms.at(uniformName.data()), value.x, value.y, value.z);
}

/***********************************************************************************/
void GLShaderProgram::SetUniform(const std::string_view uniformName, const glm::vec4& value) const {
	glUniform4f(m_uniforms.at(uniformName.data()), value.x, value.y, value.z, value.w);
}

/***********************************************************************************/
void GLShaderProgram::SetUniform(const std::string_view uniformName, const glm::mat3x3& value) const {
	glUniformMatrix3fv(m_uniforms.at(uniformName.data()), 1, GL_FALSE, value_ptr(value));
}

/***********************************************************************************/
void GLShaderProgram::SetUniform(const std::string_view uniformName, const glm::mat4x4& value) const {
	glUniformMatrix4fv(m_uniforms.at(uniformName.data()), 1, GL_FALSE, value_ptr(value));
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
