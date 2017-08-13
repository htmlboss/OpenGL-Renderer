#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <unordered_map>
#include <array>

class GLShader;

class GLShaderProgram {
public:
	GLShaderProgram(const std::string_view programName, const std::initializer_list<GLShader> shaders);
	~GLShaderProgram();

	void AddUniforms(const std::initializer_list<std::string_view> uniforms);
	void Bind() const;
	void DeleteProgram() const;

	GLShaderProgram& SetUniformi(const std::string_view uniformName, const int value);
	GLShaderProgram& SetUniformf(const std::string_view uniformName, const float value);
	GLShaderProgram& SetUniform(const std::string_view uniformName, const glm::ivec2& value);
	GLShaderProgram& SetUniform(const std::string_view uniformName, const glm::vec2& value);
	GLShaderProgram& SetUniform(const std::string_view uniformName, const glm::vec3& value);
	GLShaderProgram& SetUniform(const std::string_view uniformName, const glm::vec4& value);
	GLShaderProgram& SetUniform(const std::string_view uniformName, const glm::mat3x3& value);
	GLShaderProgram& SetUniform(const std::string_view uniformName, const glm::mat4x4& value);

private:
	void linkAndValidate();

	std::unordered_map<std::string_view, int> m_uniforms;

	unsigned int m_programID;
	const std::string m_programName;

	//  To check for compile-time errors
	int m_success;
	std::array<char, 1024> m_infoLog;
};
