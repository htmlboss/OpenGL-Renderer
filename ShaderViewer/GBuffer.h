#pragma once
#include <GL/glew.h>

class GBuffer {
public:
	GBuffer(const size_t width, const size_t height);
	~GBuffer();

	void BindGBuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer); }
	void UnBindGBuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	void BindTextures() const;
private:
	GLuint m_gBuffer, m_gPosition, m_gNormal, m_gDiffuseSpec, m_rboDepth;
};

