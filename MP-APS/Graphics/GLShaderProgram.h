#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>

#include <unordered_map>
#include <string>

class GLShaderProgram {

public:
	GLShaderProgram(const std::string& programName, const GLuint programID);
	~GLShaderProgram();

	GLShaderProgram(GLShaderProgram&& other) {
		std::swap(m_uniforms, other.m_uniforms);
		std::swap(m_programID, other.m_programID);
		std::swap(m_programName, other.m_programName);
	}

	GLShaderProgram& operator=(GLShaderProgram other) noexcept(true) {
		std::swap(m_uniforms, other.m_uniforms);
		std::swap(m_programID, other.m_programID);
		std::swap(m_programName, other.m_programName);
		return *this;
	}

	// Disable copying to avoid accidentally calling the destructor
	// on the original instance which causes the program to be
	// deleted on the GPU, but all of the original instance's data
	// is dumbly copied over to the new instance (including non-zero
	// programID) but in fact the program ID is now a dangling reference
	GLShaderProgram(const GLShaderProgram&) = delete;
	GLShaderProgram& operator=(const GLShaderProgram&) = delete;

	void Bind() const;
	void DeleteProgram();

	GLShaderProgram& SetUniformi(const std::string& uniformName, const int value);
	GLShaderProgram& SetUniformf(const std::string& uniformName, const float value);
	GLShaderProgram& SetUniform(const std::string& uniformName, const glm::ivec2& value);
	GLShaderProgram& SetUniform(const std::string& uniformName, const glm::vec2& value);
	GLShaderProgram& SetUniform(const std::string& uniformName, const glm::vec3& value);
	GLShaderProgram& SetUniform(const std::string& uniformName, const glm::vec4& value);
	GLShaderProgram& SetUniform(const std::string& uniformName, const glm::mat3x3& value);
	GLShaderProgram& SetUniform(const std::string& uniformName, const glm::mat4x4& value);

	auto GetProgramName() const noexcept { return m_programName; }

private:
	void getUniforms();

	std::unordered_map<std::string, int> m_uniforms;

    static const GLuint noId = 0;
	GLuint m_programID{ noId };
	std::string m_programName;
};
