#pragma once

#include <glad/glad.h>

class GLVertexArray {

public:
	enum BufferType : int {
		ARRAY = GL_ARRAY_BUFFER,
		ELEMENT = GL_ELEMENT_ARRAY_BUFFER
	};

	enum DrawMode : int {
		STATIC = GL_STATIC_DRAW,
		DYNAMIC = GL_DYNAMIC_DRAW,
		STREAM = GL_STREAM_DRAW
	};

	void Init();
	void AttachBuffer(const BufferType type, const size_t size, const DrawMode mode, const void* data);
	void Bind() const;
	void EnableAttribute(const GLuint index, const int size, const GLuint offset, const void* data);
	void Delete();

private:
	GLuint m_vao{ 0 };
};
