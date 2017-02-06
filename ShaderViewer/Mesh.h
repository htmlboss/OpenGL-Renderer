#pragma once

#include <vector>
#include <GL/glew.h>

#include "Texture.h"
#include "Shader.h"

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
	glm::vec3 Tangent;
};

class Mesh {
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures);
	~Mesh();
	
	void SetInstancing(const std::initializer_list<glm::vec3>& args);
	void Draw(Shader& shader);
	void DrawInstanced(Shader& shader);

private:
	void bindTextures(Shader &shader);
	void setupMesh();

	bool m_uniformsAdded;

	GLuint m_vao, m_vbo, m_ebo, m_instanceVBO;
	std::vector<Vertex> m_vertices;
	std::vector<GLuint> m_indices;
	std::vector<Texture> m_textures;
	std::vector<glm::vec3> m_instanceOffsets;
};

