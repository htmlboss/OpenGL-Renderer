#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GLShader.h"

#include <map>
#include <string_view>
#include <memory>

class GLShaderProgram {
public:
	GLShaderProgram(const std::string_view programName, std::initializer_list<GLShader> shaders);

	void Bind() const;

	void DeleteProgram() const;

	void AddUniform(const std::string_view uniform);
	void AddUniforms(const std::initializer_list<std::string_view> uniforms);

	void SetUniformi(const std::string_view uniformName, const GLint value) {
		glUniform1i(m_uniformMap.at(uniformName.data()), value);
	}

	void SetUniformf(const std::string_view uniformName, const GLfloat value) {
		glUniform1f(m_uniformMap.at(uniformName.data()), value);
	}

	void SetUniform(const std::string_view uniformName, const glm::vec3& value) {
		glUniform3f(m_uniformMap.at(uniformName.data()), value.x, value.y, value.z);
	}

	void SetUniform(const std::string_view uniformName, const glm::vec4& value) {
		glUniform4f(m_uniformMap.at(uniformName.data()), value.x, value.y, value.z, value.w);
	}

	void SetUniform(const std::string_view uniformName, const glm::mat3x3& value) {
		glUniformMatrix3fv(m_uniformMap.at(uniformName.data()), 1, GL_FALSE, value_ptr(value));
	}

	void SetUniform(const std::string_view uniformName, const glm::mat4x4& value) {
		glUniformMatrix4fv(m_uniformMap.at(uniformName.data()), 1, GL_FALSE, value_ptr(value));
	}

private:
	void linkAndValidate();

	std::map<std::string_view, GLint> m_uniformMap;

	GLuint m_programID;
	const std::string m_programName;

	//  To check for compile-time errors
	GLint m_success;
	std::array<GLchar, 1024> m_infoLog;
};

using GLShaderProgramPtr = std::unique_ptr<GLShaderProgram>;
