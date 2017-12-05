#pragma once

#include "Vertex.h"
#include "Graphics/GLTexture.h"
#include "Graphics/GLShaderProgram.h"
#include "Graphics/GLVertexArray.h"

#include <vector>

// TODO: Add function to reset mesh (clear OpenGL references etc)
/***********************************************************************************/
class Mesh {
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<GLTexture>& textures);

	void Clear();

	void BindTextures(GLShaderProgram* shader);

	auto GetIndexCount() const noexcept { return m_indexCount; }
	auto GetVAO() const noexcept { return m_vao; }
	auto GetTriangleCount() const noexcept { return m_indexCount / 3; }
	auto GetTextureCount() const noexcept { return m_textures.size(); }

private:
	void setupMesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
	std::size_t m_indexCount;

	GLVertexArray m_vao;
	std::vector<GLTexture> m_textures;
};
