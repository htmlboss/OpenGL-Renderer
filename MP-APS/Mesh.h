#pragma once

#include <glad/glad.h>

#include "Vertex.h"
#include "GL/GLTexture.h"
#include "GL/GLShaderProgram.h"
#include "GL/GLVertexArray.h"

#include <vector>

/***********************************************************************************/
class Mesh {
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<GLTexture>& textures);

	void SetInstancing(const std::initializer_list<glm::vec3>& args);
	void Draw(GLShaderProgram* shader);
	void DrawInstanced(GLShaderProgram* shader);

private:
	void bindTextures(GLShaderProgram* shader);
	void setupMesh();

	bool m_uniformsAdded;

	GLVertexArray m_vao;

	std::vector<Vertex> m_vertices;
	std::vector<GLuint> m_indices;
	std::vector<GLTexture> m_textures;
	std::vector<glm::vec3> m_instanceOffsets;
};
