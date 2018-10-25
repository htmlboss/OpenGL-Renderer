#pragma once

#include <glad/glad.h>

#include <string_view>

// Helper class to encapsulate common FBO stuff and clean up repeated code.
class GLFramebuffer {

public:

	enum class AttachmentType : decltype(GL_COLOR_ATTACHMENT0) {
		COLOR0 = GL_COLOR_ATTACHMENT0,
		COLOR1 = GL_COLOR_ATTACHMENT1,
		COLOR2 = GL_COLOR_ATTACHMENT2,
		COLOR3 = GL_COLOR_ATTACHMENT3,
		COLOR4 = GL_COLOR_ATTACHMENT4,

		DEPTH = GL_DEPTH_ATTACHMENT,
		STENCIL = GL_STENCIL_ATTACHMENT
	};

	enum class BufferBitMasks : decltype(GL_COLOR_BUFFER_BIT) {
		COLOR = GL_COLOR_BUFFER_BIT,
		DEPTH = GL_DEPTH_BUFFER_BIT,
		STENCIL = GL_STENCIL_BUFFER_BIT
	};

	enum class GLBuffer : decltype(GL_NONE) {
		NONE = GL_NONE,
		FRONT = GL_FRONT,
		BACK = GL_BACK,
		LEFT = GL_LEFT,
		RIGHT = GL_RIGHT,
		FRONT_RIGHT = GL_FRONT_RIGHT,
		BACK_LEFT = GL_BACK_LEFT
	};

	// Initialize framebuffer with OpenGL
	void Init(const std::string_view name);
	// Tell OpenGL to delete this framebuffer and underlying attachmants
	void Delete();
	// Deletes and re-initializes this framebuffer. Useful when needing to resize.
	void Reset();

	void AttachTexture(const GLuint& texID, const AttachmentType colorAttach) const;
	void AttachRenderBuffer(const GLuint& rboID, const AttachmentType type) const;
	void Bind() const;
	void Unbind() const;
	void DrawBuffers(const unsigned int attachments[]) const;
	void DrawBuffer(const GLBuffer buffer) const;
	void ReadBuffer(const GLBuffer buffer) const;

private:
	void checkErrors() const;

	GLuint m_fboID{ 0 };

	std::string_view m_name;
};