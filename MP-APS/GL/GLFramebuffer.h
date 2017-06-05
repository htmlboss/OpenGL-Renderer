#pragma once
#include "../Interfaces/IRenderComponent.h"

#include <glad/glad.h>

#include <memory>

// Helper class to encapsulate common FBO stuff and clean up repeated code.
class GLFramebuffer : IRenderComponent {
public:
	GLFramebuffer(const std::string_view name, const std::size_t width, const std::size_t height);

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

	void Delete();
	void Reset(const std::size_t width, const std::size_t height);

	void AttachTexture(const GLuint texID, const AttachmentType colorAttach) const;
	void AttachRenderBuffer(const GLuint rboID, const AttachmentType type) const;
	void Bind() const;
	void Unbind() const;
	void Blit(const BufferBitMasks bufferBit, const GLint targetID) const;

	void Resize(const std::size_t width, const std::size_t height) override;

private:
	void checkErrors() const;

	GLuint m_fbo;
};

using GLFramebufferPtr = std::unique_ptr<GLFramebuffer>;