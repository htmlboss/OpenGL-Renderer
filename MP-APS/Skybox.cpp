#include "Skybox.h"

#include "ResourceManager.h"
#include "Graphics/GLShader.h"

#include <glm/gtc/matrix_transform.hpp>

#include <array>

/***********************************************************************************/
Skybox::Skybox(const std::string_view hdrPath, const std::size_t resolution) : m_skyboxShader("Skybox Shader", {GLShader("Data/Shaders/skyboxvs.glsl", GL_VERTEX_SHADER),
																												GLShader("Data/Shaders/skyboxps.glsl", GL_FRAGMENT_SHADER) }) {

	const std::array<float, 108> vertices {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		1.0f,  1.0f, -1.0f,
		1.0f,  1.0f,  1.0f,
		1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f, -1.0f,
		1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		1.0f, -1.0f,  1.0f
	};

	// No seams at cubemap edges
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	// Fill buffer
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);
	// link vertex attributes
	glBindVertexArray(m_vao);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	const auto hdrTexture = ResourceManager::GetInstance().LoadHDRI(hdrPath);

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
	const glm::mat4 captureViews[] {
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// Convert HDR equirectangular environment map to cubemap
	auto convertToCubemapShader = GLShaderProgram("Equirectangular to cubemap shader", {GLShader("Data/Shaders/cubemapconvertervs.glsl", GL_VERTEX_SHADER),
																						GLShader("Data/Shaders/cubemapconverterps.glsl", GL_FRAGMENT_SHADER) });
	convertToCubemapShader.Bind();
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

		renderCube();
	}

	glDeleteTextures(1, &hdrTexture);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Precompute irradiance cubemap.
	glGenTextures(1, &m_irradianceMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_irradianceMap);
	for (auto i = 0; i < 6; ++i) {
		// Convoluting a cubemap purposefully scrubs out the fine details so we only need a low-res image (default 32)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution / 16, resolution / 16, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	auto irradianceShader = GLShaderProgram("Irradiance Shader", {	GLShader("Data/Shaders/cubemapconvertervs.glsl", GL_VERTEX_SHADER),
																	GLShader("Data/Shaders/irradianceConvolutionps.glsl", GL_FRAGMENT_SHADER) });
	glBindFramebuffer(GL_FRAMEBUFFER, m_envMapFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution / 16, resolution / 16);

	// solve diffuse integral by convolution to create an irradiance (cube)map.
	irradianceShader.Bind();
	irradianceShader.SetUniformi("environmentMap", 0).SetUniform("projection", captureProjection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envMap);

	glViewport(0, 0, resolution / 16, resolution / 16);
	glBindFramebuffer(GL_FRAMEBUFFER, m_envMapFBO);
	for (unsigned int i = 0; i < 6; ++i) {
		irradianceShader.SetUniform("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();

	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

/***********************************************************************************/
void Skybox::Draw() {
	m_skyboxShader.Bind();

	glActiveTexture(GL_TEXTURE0);
	m_skyboxShader.SetUniformi("environmentMap", 0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envMap);

	// Skybox Cube
	renderCube();
}

/***********************************************************************************/
void Skybox::renderCube() {
	glBindVertexArray(m_vao);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
