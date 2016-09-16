#pragma once

#include <string>
#include <vector>

#include <GL\glew.h>
#include <glm\glm.hpp>

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
	void Draw(const Shader& shader);
	void DrawInstanced(const Shader& shader);

private:
	void bindTextures(const Shader &shader);
	void setupMesh();

	GLuint m_vao, m_vbo, m_ebo, m_instanceVBO;
	std::vector<Vertex> m_vertices;
	std::vector<GLuint> m_indices;
	std::vector<Texture> m_textures;
	std::vector<glm::vec3> m_instanceOffsets;
};

