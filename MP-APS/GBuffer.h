#pragma once
#include <GL/glew.h>

class GBuffer {
public:
	GBuffer(const size_t width, const size_t height);
	~GBuffer();

	void BindGBuffer() const;
	void UnBindGBuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	void BindTextures() const;

	// Write depth buffer to default framebuffer
	void BlitDepthBuffer() const;
private:
	GLuint m_gBuffer, m_gPosition, m_gNormal, m_gDiffuseSpec, m_rboDepth;
	const size_t m_width, m_height;
};

