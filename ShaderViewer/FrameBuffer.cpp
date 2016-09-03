#include "FrameBuffer.h"
#include <iostream>

/***********************************************************************************/
FrameBuffer::FrameBuffer(const GLint WindowWidth, const GLint WindowHeight, const GLboolean depthTex, const GLboolean stencilTex) :
	m_width(WindowWidth), m_height(WindowHeight) {
	
	// Generate frambuffer
	glGenFramebuffers(1, &m_fbo);
	Bind();
	
	// Generate texture
	GLenum attachment_type;
	if (!depthTex && !stencilTex) {
		attachment_type = GL_RGB;
	}
	else if (depthTex && !stencilTex) {
		attachment_type = GL_DEPTH_COMPONENT;
	}
	else if (!depthTex && stencilTex) {
		attachment_type = GL_STENCIL_INDEX;
	}

	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);
	if (!depthTex && !stencilTex) {
		glTexImage2D(GL_TEXTURE_2D, 0, attachment_type, m_width, m_height, 0, attachment_type, GL_UNSIGNED_BYTE, NULL);
	}
	// Using both a stencil and depth test, needs special format arguments
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, m_width, m_height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
	}
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glBindTexture(GL_TEXTURE_2D, 0);
	// Attach it to currently bound fbo
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureID, 0);

	glGenRenderbuffers(1, &m_rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, m_width, m_height);
	glBindRenderbuffer(GL_RENDERBUFFER, 0);
	// Attach rbo to fbo
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_rbo);

	isFrameBufferComplete();
	UnBind();
}

/***********************************************************************************/
FrameBuffer::~FrameBuffer() {
}

/***********************************************************************************/
bool FrameBuffer::isFrameBufferComplete() const {
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
		std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
		return false;
	}
	return true;
}
