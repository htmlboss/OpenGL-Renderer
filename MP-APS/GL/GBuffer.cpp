#include "GBuffer.h"

#include <array>
#include <iostream>

#include "../ResourceManager.h"

/***********************************************************************************/
GBuffer::GBuffer(const size_t width, const size_t height) : m_width(width), m_height(height),	
															m_geometryPassShader("Geometry Pass Shader", {	GLShader(ResourceManager::GetInstance().LoadTextFile("shaders/geometrypassvs.glsl"),  GLShader::ShaderType::VertexShader), 
																											GLShader(ResourceManager::GetInstance().LoadTextFile("shaders/geometrypassps.glsl"), GLShader::ShaderType::PixelShader) }), 
															m_lightingPassShader("Lighting Pass Shader", {	GLShader(ResourceManager::GetInstance().LoadTextFile("shaders/lightingpassvs.glsl"),  GLShader::ShaderType::VertexShader), 
																											GLShader(ResourceManager::GetInstance().LoadTextFile("shaders/lightingpassps.glsl"), GLShader::ShaderType::PixelShader) }) {

	m_geometryPassShader.Bind();
	m_geometryPassShader.AddUniforms({ "model", "normalMatrix", "texture_diffuse1", "texture_specular1"});
	
	m_lightingPassShader.Bind();
	m_lightingPassShader.AddUniforms({"viewPos", "gPosition", "gNormal", "gAlbedoSpec"});
	// Set samplers
	m_lightingPassShader.SetUniformi("gPosition", 0);
	m_lightingPassShader.SetUniformi("gNormal", 1);
	m_lightingPassShader.SetUniformi("gAlbedoSpec", 2);
	
	glGenFramebuffers(1, &m_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);

	// Position color buffer
	glGenTextures(1, &m_gPosition);
	glBindTexture(GL_TEXTURE_2D, m_gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);

	// Normal color buffer
	glGenTextures(1, &m_gNormal);
	glBindTexture(GL_TEXTURE_2D, m_gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0);

	// Diffuse & Specular buffer (Gonna store specular in the alpha channel)
	glGenTextures(1, &m_gDiffuseSpec);
	glBindTexture(GL_TEXTURE_2D, m_gDiffuseSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gDiffuseSpec, 0);

	// Pass along color attachments
	std::array<GLuint, 3> attachments = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(attachments.size(), attachments.data());

	// Generate render buffer
	glGenRenderbuffers(1, &m_rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);
	
	const auto err = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if ( err != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "Framebuffer not complete: " << err << '\n';
	}
}

/***********************************************************************************/
void GBuffer::Shutdown() {
	m_geometryPassShader.DeleteProgram();
	m_lightingPassShader.DeleteProgram();
}

/***********************************************************************************/
void GBuffer::BindGBuffer() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/***********************************************************************************/
void GBuffer::BindGeometryShader() const {
	m_geometryPassShader.Bind();
}

/***********************************************************************************/
void GBuffer::BindLightingShader() const {
	m_lightingPassShader.Bind();
}

/***********************************************************************************/
void GBuffer::Resize(const std::size_t width, std::size_t height) {
	m_width = width;
	m_height = height;

	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
	// Resize all FBOs and RBO
	glBindTexture(GL_TEXTURE_2D, m_gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);

	glBindTexture(GL_TEXTURE_2D, m_gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, nullptr);

	glBindTexture(GL_TEXTURE_2D, m_gDiffuseSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

	glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
}

/***********************************************************************************/
void GBuffer::SetCameraPos(const glm::vec3& pos) {
	m_lightingPassShader.SetUniform("viewPos", pos);
}

/***********************************************************************************/
void GBuffer::BindPosNorm() const {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_gNormal);
}

/***********************************************************************************/
void GBuffer::BindPosNormDiffSpec() const {

	this->BindPosNorm();
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_gDiffuseSpec);
}

/***********************************************************************************/
void GBuffer::BlitDepthBuffer() const {
	
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
