#include "Skybox.h"
#include "ResourceManager.h"
#include "GL/GLShader.h"

#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>

/***********************************************************************************/
Skybox::Skybox(const std::string_view hdrPath, const std::size_t resolution) : m_skyboxShader("Skybox Shader", {GLShader("Shaders/skyboxvs.glsl", GLShader::ShaderType::Vertex),
																												GLShader("Shaders/skyboxps.glsl", GLShader::ShaderType::Pixel) }) {
	m_skyboxShader.AddUniforms({ "environmentMap" });

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	// fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_STATIC_DRAW);
	// link vertex attributes
	glBindVertexArray(m_vao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	auto hdrTexture = ResourceManager::GetInstance().LoadHDRI(hdrPath, resolution);

	// Environment map FBO
	GLuint rbo;
	glGenFramebuffers(1, &m_envMapFBO);
	glGenRenderbuffers(1, &rbo);

	glBindFramebuffer(GL_FRAMEBUFFER, m_envMapFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

	glGenTextures(1, &m_envMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envMap);
	for (auto i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Setup projection and view matrices for capturing data onto the 6 cubemap face directions
	const auto captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	const glm::mat4 captureViews[]{
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// Convert HDR equirectangular environment map to cubemap
	auto convertToCubemapShader = GLShaderProgram("Equirectangular to cubemap shader", {GLShader("Shaders/cubemapconvertervs.glsl", GLShader::ShaderType::Vertex),
																						GLShader("Shaders/cubemapconverterps.glsl", GLShader::ShaderType::Pixel) });
	convertToCubemapShader.Bind();
	convertToCubemapShader.AddUniforms({ "projection", "view", "equirectangularMap" });
	convertToCubemapShader.SetUniformi("equirectangularMap", 0).SetUniform("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, hdrTexture);

	glViewport(0, 0, resolution, resolution);
	glBindFramebuffer(GL_FRAMEBUFFER, m_envMapFBO);

	for (auto i = 0; i < 6; ++i) {
		convertToCubemapShader.SetUniform("view", captureViews[i]);
		// Attach environment texture to FBO
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// rendercube
		glBindVertexArray(m_vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}

	glDeleteTextures(1, &hdrTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

/***********************************************************************************/
void Skybox::Draw() {
	m_skyboxShader.Bind();

	// Skybox Cube
	glBindVertexArray(m_vao);
	glActiveTexture(GL_TEXTURE0);
	m_skyboxShader.SetUniformi("environmentMap", 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envMap);
	glDrawArrays(GL_TRIANGLES, 0, 36);
}
