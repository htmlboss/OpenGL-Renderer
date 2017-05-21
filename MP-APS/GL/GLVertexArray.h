#pragma once
#include <glad/glad.h>

#include <cstddef>

class GLVertexArray {
public:
	GLVertexArray();

	enum class BufferType {
		ARRAY = GL_ARRAY_BUFFER,
		ELEMENT = GL_ELEMENT_ARRAY_BUFFER
	};

	enum class DrawMode {
		STATIC = GL_STATIC_DRAW,
		DYNAMIC = GL_DYNAMIC_DRAW,
		STREAM = GL_STREAM_DRAW
	};

	void AttachBuffer(const BufferType type, const std::size_t size, const DrawMode mode,const void* data);
	void Bind() const;
	void EnableAttribute(const unsigned int index, const int size, const unsigned int offset, const void* data);

private:
	unsigned int m_vao;
};

