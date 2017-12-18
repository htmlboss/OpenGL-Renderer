#include "Skybox.h"

#include "ResourceManager.h"
#include "Graphics/GLShader.h"
#include "Graphics/GLShaderProgram.h"

#include <glm/gtc/matrix_transform.hpp>

#include <array>

/***********************************************************************************/
void Skybox::Init(const std::string_view hdrPath, const std::size_t resolution) {

	const std::array<Vertex, 4> screenQuadVertices{
		// Positions				// GLTexture Coords
		Vertex({ -1.0f, 1.0f, 0.0f },{ 0.0f, 1.0f }),
		Vertex({ -1.0f, -1.0f, 0.0f },{ 0.0f, 0.0f }),
		Vertex({ 1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f }),
		Vertex({ 1.0f, -1.0f, 0.0f },{ 1.0f, 0.0f })
	};

	m_quadVAO.Init();
	m_quadVAO.Bind();
	m_quadVAO.AttachBuffer(GLVertexArray::BufferType::ARRAY,
		sizeof(Vertex) * screenQuadVertices.size(),
		GLVertexArray::DrawMode::STATIC,
		screenQuadVertices.data());
	m_quadVAO.EnableAttribute(0, 3, sizeof(Vertex), nullptr);
	m_quadVAO.EnableAttribute(1, 2, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));

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

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS); // No seams at cubemap edges

	glGenVertexArrays(1, &m_cubeVAO);
	glBindVertexArray(m_cubeVAO);

	unsigned int vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

	// Environment map FBO
	glGenFramebuffers(1, &m_envMapFBO);
	unsigned int envMapRBO;
	glGenRenderbuffers(1, &envMapRBO);

	glBindFramebuffer(GL_FRAMEBUFFER, m_envMapFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, envMapRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, envMapRBO);

	const auto hdrTexture = ResourceManager::GetInstance().LoadHDRI(hdrPath);

	glGenTextures(1, &m_envCubemap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
	for (auto i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution, resolution, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // enable pre-filter mipmap sampling (helps against bright dot artifacts)
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Setup projection and view matrices for capturing data onto the 6 cubemap face directions
	const auto captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
	const glm::mat4 captureViews[] {
		glm::lookAt(glm::vec3(0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		glm::lookAt(glm::vec3(0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
	};

	// Convert HDR equirectangular environment map to cubemap
	auto convertToCubemapShader = GLShaderProgram("Equirectangular to cubemap shader", {	GLShader("Data/Shaders/cubemapvs.glsl", GL_VERTEX_SHADER),
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
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_envCubemap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}

	glDeleteTextures(1, &hdrTexture);
	convertToCubemapShader.DeleteProgram();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Generate mipmaps from first mip face (again to reduce bright dots)
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

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

	glBindFramebuffer(GL_FRAMEBUFFER, m_envMapFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, envMapRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution / 16, resolution / 16);

	// Solve diffuse integral by convolution to create an irradiance cubemap
	auto irradianceShader = GLShaderProgram("Irradiance Shader", {  GLShader("Data/Shaders/cubemapvs.glsl", GL_VERTEX_SHADER),
																	GLShader("Data/Shaders/irradianceConvolutionps.glsl", GL_FRAGMENT_SHADER) });
	irradianceShader.Bind();
	irradianceShader.SetUniformi("environmentMap", 0).SetUniform("projection", captureProjection);
	
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);

	glViewport(0, 0, resolution / 16, resolution / 16);
	glBindFramebuffer(GL_FRAMEBUFFER, m_envMapFBO);
	for (unsigned int i = 0; i < 6; ++i) {
		irradianceShader.SetUniform("view", captureViews[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_irradianceMap, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderCube();
	}
	irradianceShader.DeleteProgram();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create a pre-filter cubemap, and re-scale capture FBO to pre-filter scale
	glGenTextures(1, &m_prefilterMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_prefilterMap);
	for (auto i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, resolution / 4, resolution / 4, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); // Guess what this is for?? :P
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Generate mipmaps for the cubemap so OpenGL automatically allocates the required memory.
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	// Run quasi monte-carlo simulation on the environment lighting to create a prefilter cubemap (since we can't integrate over infinite directions).
	// Pre-filter the environment map with different roughness values over multiple mipmap levels
	auto prefilterShader = GLShaderProgram("Pre-filter Shader", {	GLShader("Data/Shaders/cubemapvs.glsl", GL_VERTEX_SHADER),
																	GLShader("Data/Shaders/prefilterps.glsl", GL_FRAGMENT_SHADER)});
	prefilterShader.Bind();
	prefilterShader.SetUniformi("environmentMap", 0).SetUniform("projection", captureProjection);// .SetUniformf("resolution", resolution);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);

	glBindFramebuffer(GL_FRAMEBUFFER, m_envMapFBO);
	const unsigned int maxMipLevels = 5;
	for (unsigned int mipLevel = 0; mipLevel < maxMipLevels; ++mipLevel) {
		// Resize framebuffer according to mip-level size.
		const unsigned int mipWidth = (resolution / 4) * std::pow(0.5, mipLevel);
		const unsigned int mipHeight = (resolution / 4) * std::pow(0.5, mipLevel);
		glBindRenderbuffer(GL_RENDERBUFFER, envMapRBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, mipWidth, mipHeight);
		glViewport(0, 0, mipWidth, mipHeight);

		const float roughness = static_cast<float>(mipLevel) / static_cast<float>((maxMipLevels - 1));
		prefilterShader.SetUniformf("roughness", roughness);
		// For 6 cubemap faces
		for (auto i = 0; i < 6; ++i) {
			prefilterShader.SetUniform("view", captureViews[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_prefilterMap, mipLevel);
			
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			renderCube();
		}
	}
	prefilterShader.DeleteProgram();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Generate 2D LUT from BRDF equations
	glGenTextures(1, &m_brdfLUT);

	glBindTexture(GL_TEXTURE_2D, m_brdfLUT);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RG16F, resolution, resolution, 0, GL_RG, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Reconfigure capture framebuffer object and render screen-space quad with BRDF shader
	glBindFramebuffer(GL_FRAMEBUFFER, m_envMapFBO);
	glBindRenderbuffer(GL_RENDERBUFFER, envMapRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, resolution, resolution);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_brdfLUT, 0);

	auto brdfShader = GLShaderProgram("BRDF Shader", {	GLShader("Data/Shaders/brdfvs.glsl", GL_VERTEX_SHADER), 
														GLShader("Data/Shaders/brdfps.glsl", GL_FRAGMENT_SHADER)});
	brdfShader.Bind();
	glViewport(0, 0, resolution, resolution);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_quadVAO.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	
	brdfShader.DeleteProgram();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/***********************************************************************************/
void Skybox::Draw() {
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_envCubemap);
	renderCube();
}

/***********************************************************************************/
void Skybox::renderCube() {
	glBindVertexArray(m_cubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}
