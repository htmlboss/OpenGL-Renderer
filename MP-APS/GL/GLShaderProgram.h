#pragma once
#include <glad/glad.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <unordered_map>
#include <array>

class GLShader;

class GLShaderProgram {
public:
	GLShaderProgram(const std::string_view programName, std::initializer_list<GLShader> shaders);

	void AddUniforms(const std::initializer_list<std::string_view> uniforms);
	void Bind() const;
	void DeleteProgram() const;

	void SetUniformi(const std::string_view uniformName, const GLint value) const;
	void SetUniformf(const std::string_view uniformName, const GLfloat value) const;
	void SetUniform(const std::string_view uniformName, const glm::ivec2& value) const;
	void SetUniform(const std::string_view uniformName, const glm::vec3& value) const;
	void SetUniform(const std::string_view uniformName, const glm::vec4& value) const;
	void SetUniform(const std::string_view uniformName, const glm::mat3x3& value) const;
	void SetUniform(const std::string_view uniformName, const glm::mat4x4& value) const;

private:
	void linkAndValidate();

	std::unordered_map<std::string_view, GLint> m_uniforms;

	GLuint m_programID;
	const std::string m_programName;

	//  To check for compile-time errors
	GLint m_success;
	std::array<GLchar, 1024> m_infoLog;
};
