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
};

class Mesh {
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures);
	~Mesh();
	
	void Draw(const Shader& shader);

private:
	void setupMesh();

	GLuint m_vao, m_vbo, m_ebo;
	std::vector<Vertex> m_vertices;
	std::vector<GLuint> m_indices;
	std::vector<Texture> m_textures;
};

