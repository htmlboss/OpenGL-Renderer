#include "GLVertexArray.h"

/***********************************************************************************/
void GLVertexArray::Init() noexcept {
	glGenVertexArrays(1, &m_vao);
}

/***********************************************************************************/
void GLVertexArray::AttachBuffer(const BufferType type, const size_t size, const DrawMode mode, const void* data) noexcept {

	GLuint buffer;
	glGenBuffers(1, &buffer);

	glBindBuffer(type, buffer);
	glBufferData(type, size, data, mode);
}

/***********************************************************************************/
void GLVertexArray::Bind() const noexcept {
	glBindVertexArray(m_vao);
}

/***********************************************************************************/
void GLVertexArray::Delete() noexcept {
	glDeleteVertexArrays(1, &m_vao);
}

/***********************************************************************************/
void GLVertexArray::EnableAttribute(const unsigned int index, const int size, const unsigned int offset, const void* data) noexcept {
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, offset, data);
}
