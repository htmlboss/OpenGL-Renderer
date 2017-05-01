#pragma once

#include <glad/glad.h>

#include <memory>

// Helper class to encapsulate common FBO stuff and clean up repeated code.
class GLFramebuffer {
public:
	GLFramebuffer(const std::size_t width, const std::size_t height);
	~GLFramebuffer() = default;

	enum class AttachmentType {
		COLOR0 = GL_COLOR_ATTACHMENT0,
		COLOR1 = GL_COLOR_ATTACHMENT1,
		COLOR2 = GL_COLOR_ATTACHMENT2,
		COLOR3 = GL_COLOR_ATTACHMENT3,
		COLOR4 = GL_COLOR_ATTACHMENT4,

		DEPTH = GL_DEPTH_ATTACHMENT,
		STENCIL = GL_STENCIL_ATTACHMENT
	};

	enum class BufferBitMasks {
		COLOR = GL_COLOR_BUFFER_BIT,
		DEPTH = GL_DEPTH_BUFFER_BIT,
		STENCIL = GL_STENCIL_BUFFER_BIT
	};

	void AttachTexture(const GLuint texID, const AttachmentType colorAttach) const;
	void AttachRenderBuffer(const GLuint rboID, const AttachmentType type) const;
	void Bind() const;
	void Blit(const BufferBitMasks bufferBit, const GLint targetID) const;

private:
	static void checkErrors();

	std::size_t m_width, m_height;
	GLuint m_fbo;
};

using GLFramebufferPtr = std::unique_ptr<GLFramebuffer>;