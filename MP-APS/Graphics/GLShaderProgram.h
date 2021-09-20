#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <unordered_map>
#include <array>
#include <string>
#include <vector>

class GLShader;

class GLShaderProgram {
public:
	GLShaderProgram(const std::string_view programName, const std::vector<GLShader>& shaders);
	~GLShaderProgram();

	void Bind() const;
	void DeleteProgram() const;

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
	bool linkAndValidate();
	void getUniforms();

	std::unordered_map<std::string, int> m_uniforms;

	unsigned int m_programID;
	const std::string m_programName;

	//  To check for compile-time errors
	int m_success;
	std::array<char, 1024> m_infoLog;
};
