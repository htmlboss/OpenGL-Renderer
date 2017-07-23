#pragma once

#include "Vertex.h"
#include "GL/GLTexture.h"
#include "GL/GLShaderProgram.h"
#include "GL/GLVertexArray.h"

#include <vector>

/***********************************************************************************/
struct GLRenderData {
	explicit GLRenderData() {}
	explicit GLRenderData(const std::vector<GLuint>& indices) : VAO(), Indices(indices) {}

	GLVertexArray VAO;
	std::vector<GLuint> Indices;
};

/***********************************************************************************/
class Mesh {
public:
	explicit Mesh() {}
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<GLTexture>& textures);

	void BindTextures(GLShaderProgram* shader);

	auto GetRenderData() const noexcept { return m_renderData; }

private:
	void setupMesh();

	GLRenderData m_renderData;

	std::vector<Vertex> m_vertices;
	std::vector<GLTexture> m_textures;
};
