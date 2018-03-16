#pragma once

#include <glad/glad.h>

class GLVertexArray {
public:
	GLVertexArray() = default;

	enum BufferType : int {
		ARRAY = GL_ARRAY_BUFFER,
		ELEMENT = GL_ELEMENT_ARRAY_BUFFER
	};

	enum DrawMode : int {
		STATIC = GL_STATIC_DRAW,
		DYNAMIC = GL_DYNAMIC_DRAW,
		STREAM = GL_STREAM_DRAW
	};

	void Init() noexcept;
	void AttachBuffer(const BufferType type, const size_t size, const DrawMode mode, const void* data) noexcept;
	void Bind() const noexcept;
	void EnableAttribute(const GLuint index, const int size, const GLuint offset, const void* data) noexcept;
	void Delete() noexcept;

private:
	GLuint m_vao;
};
