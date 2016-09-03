#pragma once

#define GLEW_STATIC
#include <gl/glew.h>

class FrameBuffer {
public:
	FrameBuffer(const GLint WindowWidth, const GLint WindowHeight, const GLboolean depthTex, const GLboolean stencilTex);
	virtual ~FrameBuffer();

	// Bind Framebuffer
	void Bind() const { glBindFramebuffer(GL_FRAMEBUFFER, m_fbo); };
	void UnBind() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); };

	// Get Framebuffer ID
	GLuint GetID() const { return m_textureID; }

private:

	bool isFrameBufferComplete() const;

	GLuint m_fbo, m_texture, m_rbo, m_textureID;
	GLint m_width, m_height;
};