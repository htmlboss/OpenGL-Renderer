#include "GLShaderProgram.h"

#include <cassert>
#include <iostream>

/***********************************************************************************/
GLShaderProgram::GLShaderProgram(const std::string& programName, const GLuint programID) :
	m_programName(programName), m_programID(programID)
{
	getUniforms();
}

/***********************************************************************************/
GLShaderProgram::~GLShaderProgram() {
	DeleteProgram();
}

/***********************************************************************************/
void GLShaderProgram::Bind() const {
	assert(m_programID != GLShaderProgram::noId);

	glUseProgram(m_programID);
}

/***********************************************************************************/
void GLShaderProgram::DeleteProgram() {
    if (m_programID != GLShaderProgram::noId) {
		std::cout << "Deleting program: " << m_programName << '\n';
		glDeleteProgram(m_programID);
                m_programID = GLShaderProgram::noId;
	}
}

/***********************************************************************************/
GLShaderProgram& GLShaderProgram::SetUniformi(const std::string& uniformName, const int value) {
	glUniform1i(m_uniforms.at(uniformName), value);

	return *this;
}

/***********************************************************************************/
GLShaderProgram& GLShaderProgram::SetUniformf(const std::string& uniformName, const float value) {
	glUniform1f(m_uniforms.at(uniformName), value);

	return *this;
}

/***********************************************************************************/
GLShaderProgram& GLShaderProgram::SetUniform(const std::string& uniformName, const glm::ivec2& value) {
	glUniform2iv(m_uniforms.at(uniformName), 1, &value[0]);

	return *this;
}

/***********************************************************************************/
GLShaderProgram& GLShaderProgram::SetUniform(const std::string& uniformName, const glm::vec2& value) {
	glUniform2f(m_uniforms.at(uniformName), value.x, value.y);

	return *this;
}

/***********************************************************************************/
GLShaderProgram& GLShaderProgram::SetUniform(const std::string& uniformName, const glm::vec3& value) {
	glUniform3f(m_uniforms.at(uniformName), value.x, value.y, value.z);

	return *this;
}

/***********************************************************************************/
GLShaderProgram& GLShaderProgram::SetUniform(const std::string& uniformName, const glm::vec4& value) {
	glUniform4f(m_uniforms.at(uniformName), value.x, value.y, value.z, value.w);

	return *this;
}

/***********************************************************************************/
GLShaderProgram& GLShaderProgram::SetUniform(const std::string& uniformName, const glm::mat3x3& value) {
	glUniformMatrix3fv(m_uniforms.at(uniformName), 1, GL_FALSE, value_ptr(value));

	return *this;
}

/***********************************************************************************/
GLShaderProgram& GLShaderProgram::SetUniform(const std::string& uniformName, const glm::mat4x4& value) {
	glUniformMatrix4fv(m_uniforms.at(uniformName), 1, GL_FALSE, value_ptr(value));

	return *this;
}

/***********************************************************************************/
void GLShaderProgram::getUniforms() {
	
	int total = -1;
	glGetProgramiv(m_programID, GL_ACTIVE_UNIFORMS, &total);
	for (auto i = 0; i < total; ++i) {
		auto name_len = -1, num = -1;
		GLenum type = GL_ZERO;
		char name[100];
		glGetActiveUniform(m_programID, static_cast<GLuint>(i), sizeof(name) - 1, &name_len, &num, &type, name);
		name[name_len] = 0;

		const auto nameStr = std::string(name);

		// TODO: Filter out uniform block members using glGetActiveUniformsiv
		m_uniforms.try_emplace(nameStr, glGetUniformLocation(m_programID, name));
	}
}
