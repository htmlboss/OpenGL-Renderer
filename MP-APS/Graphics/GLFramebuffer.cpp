#include "GLFramebuffer.h"

#include <iostream>

/***********************************************************************************/
void GLFramebuffer::Init(const std::string_view name) {
	m_name = name;
	Reset();
}

/***********************************************************************************/
void GLFramebuffer::Delete() {
	if (m_fboID) {
		glDeleteFramebuffers(1, &m_fboID);
		m_fboID = 0;
	}
}

/***********************************************************************************/
void GLFramebuffer::Reset() {
	Delete();
	glGenFramebuffers(1, &m_fboID);

	checkErrors();
}

/***********************************************************************************/
void GLFramebuffer::AttachTexture(const GLuint& texID, const AttachmentType colorAttach) const {
	glFramebufferTexture2D(GL_FRAMEBUFFER, static_cast<int>(colorAttach), GL_TEXTURE_2D, texID, 0);
	checkErrors();
}

/***********************************************************************************/
void GLFramebuffer::AttachRenderBuffer(const GLuint& rboID, const AttachmentType type) const {
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, static_cast<int>(type), GL_RENDERBUFFER, rboID);
	checkErrors();
}

/***********************************************************************************/
void GLFramebuffer::Bind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
}

/***********************************************************************************/
void GLFramebuffer::Unbind() const {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

/***********************************************************************************/
void GLFramebuffer::DrawBuffers(const unsigned int attachments[]) const {
	glDrawBuffers(sizeof(attachments) / sizeof(attachments[0]), attachments);
}

/***********************************************************************************/
void GLFramebuffer::DrawBuffer(const GLBuffer buffer) const {
	glDrawBuffer(static_cast<int>(buffer));
	checkErrors();
}

/***********************************************************************************/
void GLFramebuffer::ReadBuffer(const GLBuffer buffer) const {
	glReadBuffer(static_cast<int>(buffer));
	checkErrors();
}

/***********************************************************************************/
void GLFramebuffer::checkErrors() const {
	const auto err = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	switch (err) {
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			std::cerr << m_name << " error: Not all framebuffer attachment points are\
								 framebuffer attachment complete. " << err << '\n';
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			std::cerr << m_name << " error: No images are attached to the framebuffer. " << err << '\n';
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			std::cerr << m_name << " error: value of GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE\
								 is GL_NONE for any color attachment point(s) named by\
								 GL_DRAW_BUFFERi. " << err << '\n';
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			std::cerr << m_name << " error: GL_READ_BUFFER is not GL_NONE and the value of\
								 GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE is GL_NONE for\
								 the color attachment point named by GL_READ_BUFFER. " << err << '\n';
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			std::cerr << m_name << " error: The combination of internal formats of the\
								 attached images violates an implementation-dependent\
								 set of restrictions. " << err << '\n';
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:
			std::cerr << m_name << " error: the value of GL_RENDERBUFFER_SAMPLES is not the\
								 same for all attached renderbuffers, or the value of\
								 GL_TEXTURE_FIXED_SAMPLE_LOCATIONS is not the same for\
								 all attached textures. " << err << '\n';
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:
			std::cerr << m_name << " error: any framebuffer attachment is layered, and any\
								 populated attachment is not layered, or if all populated\
								 color attachments are not from textures of the same target. " << err << '\n';
			break;
		default:
			break;
	}
}
