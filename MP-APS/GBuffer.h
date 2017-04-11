#pragma once
#include "glad/glad.h"

#include <cstddef>

class GBuffer {
public:
	GBuffer(const std::size_t width, const std::size_t height);
	~GBuffer();

	void BindGBuffer() const;
	void UnBindGBuffer() const { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

	void Resize(const std::size_t width, std::size_t height);

	void BindTextures() const;

	// Write depth buffer to default framebuffer
	void BlitDepthBuffer() const;
private:
	GLuint m_gBuffer, m_gPosition, m_gNormal, m_gDiffuseSpec, m_rboDepth;
	std::size_t m_width, m_height;
};

