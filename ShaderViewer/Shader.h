#pragma once
#include <GL/glew.h>
#include <GLM/gtc/type_ptr.hpp>

#include <map>
#include <array>

// Allow std::array to use operator<<
template <typename T, std::size_t N>
	std::ostream& operator<<(std::ostream& o, const std::array<T, N>& arr) {
		copy(arr.cbegin(), arr.cend(), std::ostream_iterator<T>(o, " "));		
		return o;
}

class Shader {
public:
	Shader(const char* shaderName);
	~Shader();

	enum ShaderType {
		PixelShader = GL_FRAGMENT_SHADER,
		VertexShader = GL_VERTEX_SHADER,
		GeometryShader = GL_GEOMETRY_SHADER
	};

	void AddShader(const std::string& shaderSource, const ShaderType shaderType);
	void AddUniform(const std::string& uniform);
	void AddUniforms(const std::initializer_list<std::string> uniforms);
	void Link();
	void Bind() const;

	void SetUniformi(const std::string& uniformName, const GLint value) { glUniform1i(m_uniformMap.at(uniformName), value); }
	void SetUniformf(const std::string& uniformName, const GLfloat value) { glUniform1f(m_uniformMap.at(uniformName), value); }
	void SetUniform(const std::string& uniformName, const glm::vec3& value) { glUniform3f(m_uniformMap.at(uniformName), value.x, value.y, value.z); }
	void SetUniform(const std::string& uniformName, const glm::mat4x4& value) { glUniformMatrix4fv(m_uniformMap.at(uniformName), 1, GL_FALSE, value_ptr(value)); }

private:
	GLuint compileShader(const GLchar* shaderSource, const ShaderType shaderType);

	GLuint m_program;
	bool m_linked;
	const char* m_shaderName;

	std::map<std::string, GLint> m_uniformMap;

	//  To check for compile-time errors
	GLint m_success;
	std::array<GLchar, 1024> m_infoLog;
};

