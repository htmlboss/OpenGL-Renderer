#include "GLFramebuffer.h"

#include <iostream>

/***********************************************************************************/
GLFramebuffer::GLFramebuffer(const std::string_view name, const std::size_t width, const std::size_t height) : IRenderComponent(name, width, height) {
	glGenFramebuffers(1, &m_fbo);
}

/***********************************************************************************/
void GLFramebuffer::AttachTexture(const GLuint texID, const AttachmentType colorAttach) const {
	glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<int>(colorAttach), GL_TEXTURE_2D, texID, 0);
	checkErrors();
}

/***********************************************************************************/
void GLFramebuffer::AttachRenderBuffer(const GLuint rboID, const AttachmentType type) const {
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, static_cast<int>(type), GL_RENDERBUFFER, rboID);
	checkErrors();
}

/***********************************************************************************/
void GLFramebuffer::Bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fbo);
}

/***********************************************************************************/
void GLFramebuffer::Unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/***********************************************************************************/
void GLFramebuffer::Blit(const BufferBitMasks bufferBit, const GLint targetID) const {
	Bind();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, targetID);
	glBlitFramebuffer(0, 0, m_width, m_height, 0, 0, m_width, m_height, static_cast<int>(bufferBit), GL_NEAREST);
}

/***********************************************************************************/
void GLFramebuffer::Resize(const std::size_t width, const std::size_t height) {
	m_width = width;
	m_height = height;
}
/***********************************************************************************/
void GLFramebuffer::checkErrors() const {
	const auto err = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	if (err != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << m_name << " not complete: " << err << '\n';
	}
}
