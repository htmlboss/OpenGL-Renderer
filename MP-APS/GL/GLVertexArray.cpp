#include "GLVertexArray.h"

/***********************************************************************************/
GLVertexArray::GLVertexArray() {
	glGenVertexArrays(1, &m_vao);
}

/***********************************************************************************/
void GLVertexArray::AttachBuffer(const BufferType type, const std::size_t size, const DrawMode mode, const void* data) {
	
	GLuint buffer;
	glGenBuffers(1, &buffer);

	glBindBuffer(static_cast<int>(type), buffer);
	glBufferData(static_cast<int>(type), size, data, static_cast<int>(mode));
}

/***********************************************************************************/
void GLVertexArray::Bind() const {
	glBindVertexArray(m_vao);
}

/***********************************************************************************/
void GLVertexArray::EnableAttribute(const unsigned int index, const int size, const unsigned int offset, const void* data) {
	glEnableVertexAttribArray(index);
	glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, offset, data);
}
