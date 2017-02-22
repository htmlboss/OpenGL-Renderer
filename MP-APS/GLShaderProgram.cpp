#include "GLShaderProgram.h"
#include <iostream>

/***********************************************************************************/
GLShaderProgram::GLShaderProgram(const std::string_view programName, std::initializer_list<Shader> shaders) : m_programName(programName), m_linked(false) {
	
	m_program = glCreateProgram();
	// Attach shaders
	for (auto& shader : shaders) {
		glAttachShader(m_program, shader.m_shaderID);
	}

	glLinkProgram(m_program);
	// Check for linking errors
	glGetProgramiv(m_program, GL_LINK_STATUS, &m_success);
	if (!m_success) {
		glGetProgramInfoLog(m_program, m_infoLog.size(), nullptr, m_infoLog.data());
		std::cerr << "Shader Error: " << m_programName.c_str() << " failed to link:\n" << m_infoLog.data() << std::endl;
		throw std::runtime_error("");
	}
	m_linked = true;

	// Validate
	glValidateProgram(m_program);
	glGetProgramiv(m_program, GL_VALIDATE_STATUS, &m_success);
	if (!m_success) {
		glGetProgramInfoLog(m_program, m_infoLog.size(), nullptr, m_infoLog.data());
		std::cerr << "Shader Error: " << m_programName.c_str() << " failed to validate:\n" << m_infoLog.data() << std::endl;
	}

	// Cleanup
	for (auto& shader : shaders) {
		glDetachShader(m_program, shader.m_shaderID);
		glDeleteShader(shader.m_shaderID);
	}
}

/***********************************************************************************/
GLShaderProgram::~GLShaderProgram() {
	glDeleteProgram(m_program);
}

/***********************************************************************************/
void GLShaderProgram::Bind() const {
	if (m_linked) {
		glUseProgram(m_program);
	}
	else {
		std::cerr << m_programName.c_str() << " is not linked! Failed to bind.\n";
		throw std::runtime_error("");
	}
}

/***********************************************************************************/
void GLShaderProgram::AddUniform(const std::string_view uniform) {

	const GLint uniformLoc = glGetUniformLocation(m_program, uniform.data());
	
	if (uniformLoc == -1) {
		std::cerr << "Error: " << uniform.data() << " uniform not found in shader: " << m_programName.c_str() << std::endl;
		return;
	}
	m_uniformMap.try_emplace(uniform, uniformLoc);
}

/***********************************************************************************/
void GLShaderProgram::AddUniforms(const std::initializer_list<std::string_view> uniforms) {
	
	for (const auto& it : uniforms) {
		AddUniform(it);
	}
}
