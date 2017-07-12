#include "GLPostProcess.h"
#include "GLShader.h"

/***********************************************************************************/
GLPostProcess::GLPostProcess(const size_t width, const size_t height) : IRenderComponent("GLPostProcess", width, height),
																		m_postProcessShader("Post Process Shader", {GLShader("shaders/postprocessvs.glsl", GLShader::ShaderType::VertexShader),
																								GLShader("shaders/postprocessps.glsl", GLShader::ShaderType::PixelShader)}),
																		m_hdrFBO("HDR FBO", width, height) {

	m_postProcessShader.AddUniforms({"hdrBuffer", "saturationFactor", "vibranceCoefficient", "vibranceAmount"});


	m_hdrFBO.Bind();
	GLuint rboDepth;
	glGenTextures(1, &m_hdrColorBufferTexture);
	glBindTexture(GL_TEXTURE_2D, m_hdrColorBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	// - Attach buffers
	m_hdrFBO.AttachTexture(m_hdrColorBufferTexture, GLFramebuffer::AttachmentType::COLOR0);
	m_hdrFBO.AttachRenderBuffer(rboDepth, GLFramebuffer::AttachmentType::DEPTH);
}

/***********************************************************************************/
void GLPostProcess::Resize(const size_t width, const size_t height) {
	m_width = width;
	m_height = height;

	m_hdrFBO.Reset(width, height);
	m_hdrFBO.Bind();
	GLuint rboDepth;
	glGenTextures(1, &m_hdrColorBufferTexture);
	glBindTexture(GL_TEXTURE_2D, m_hdrColorBufferTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, width, height, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);

	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);
	// - Attach buffers
	m_hdrFBO.AttachTexture(m_hdrColorBufferTexture, GLFramebuffer::AttachmentType::COLOR0);
	m_hdrFBO.AttachRenderBuffer(rboDepth, GLFramebuffer::AttachmentType::DEPTH);
}

/***********************************************************************************/
void GLPostProcess::Bind() {
	m_hdrFBO.Bind();
}

/***********************************************************************************/
void GLPostProcess::Update() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	m_postProcessShader.Bind();
	m_postProcessShader.SetUniformf("saturationFactor", m_saturation);
	m_postProcessShader.SetUniformf("vibranceAmount", m_vibrance);
	m_postProcessShader.SetUniform("vibranceCoefficient", m_coefficient);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_hdrColorBufferTexture);
}

/***********************************************************************************/
void GLPostProcess::Shutdown() const { m_postProcessShader.DeleteProgram(); }

/***********************************************************************************/
void GLPostProcess::Blit() { m_hdrFBO.Blit(GLFramebuffer::BufferBitMasks::DEPTH, 0); }
