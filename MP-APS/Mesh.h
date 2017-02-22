#pragma once

#include <vector>
#include <GL/glew.h>
#include <glm/detail/type_vec3.hpp>

#include "Texture.h"
#include "GLShaderProgram.h"

/***********************************************************************************/
struct Material {
	Material(const std::string_view Name) : name(Name) {};

	const std::string name;


	bool wireframe = false;
};

/***********************************************************************************/
struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Tangent;
	glm::vec2 TexCoords;
};

/***********************************************************************************/
class Mesh {
public:
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<Texture>& textures);
	~Mesh();
	
	void SetInstancing(const std::initializer_list<glm::vec3>& args);
	void Draw(GLShaderProgram& shader);
	void DrawInstanced(GLShaderProgram& shader);

private:
	void bindTextures(GLShaderProgram& shader);
	void setupMesh();

	bool m_uniformsAdded;

	GLuint m_vao, m_vbo, m_ebo, m_instanceVBO;
	std::vector<Vertex> m_vertices;
	std::vector<GLuint> m_indices;
	std::vector<Texture> m_textures;
	std::vector<glm::vec3> m_instanceOffsets;
};

