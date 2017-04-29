#include "Mesh.h"
#include "Utils/Utils.h"

#include <string>

Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) : m_vertices(vertices), m_indices(indices) {

	setupMesh();
}

/***********************************************************************************/
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<GLTexture>& textures) :
	m_vertices(vertices), 
	m_indices(indices), 
	m_textures(textures),
	m_uniformsAdded(false) {

	//Construct mesh
	setupMesh();
}

/***********************************************************************************/
void Mesh::SetInstancing(const std::initializer_list<glm::vec3>& args) {

	m_instanceOffsets = args;

	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * args.size(), args.begin(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Vertex Instance offset
	glEnableVertexAttribArray(3);
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(3, 1); // Tell OpenGL this is an instanced vertex attribute.
}

/***********************************************************************************/
void Mesh::Draw(GLShaderProgram* shader) {
	
	bindTextures(shader);

	// Draw mesh
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
}

/***********************************************************************************/
void Mesh::DrawInstanced(GLShaderProgram* shader) {
	
	bindTextures(shader);

	// Draw instanced mesh
	glBindVertexArray(m_vao);
	glDrawElementsInstanced(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr, m_instanceOffsets.size());
}

/***********************************************************************************/
void Mesh::bindTextures(GLShaderProgram* shader) {
	using namespace Utils;

	GLuint diffuseNr = 1;
	GLuint specularNr = 1;
	GLuint normalNr = 1;

	GLuint index = 0;
	
	for (const auto& it : m_textures) {
		glActiveTexture(GL_TEXTURE0 + index); // Activate proper texture unit before binding
											  // Retrieve texture number (the N in diffuse_textureN)
		
		const auto name = it.GetSampler();
		const auto textureNameHashed = str2int(name.c_str());
		std::string number;

		switch (textureNameHashed) {
		case str2int("texture_diffuse"):
			number = std::to_string(diffuseNr++);
			break;

		case str2int("texture_specular"):
			number = std::to_string(specularNr++);
			break;

		case str2int("texture_normal"):
			number = std::to_string(normalNr++);
			break;
		}

		shader->SetUniformi(name + number, index);
		glBindTexture(GL_TEXTURE_2D, it.GetTexture());

		++index;
	}
	
	glActiveTexture(GL_TEXTURE0);
	
}

/***********************************************************************************/
void Mesh::setupMesh() {

	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_ebo);

	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_STATIC_DRAW);

	// Vertex Attributes

	// Vertex Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);
	// Vertex GLTexture Coords
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, TexCoords)));
	// Vertex Normals
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, Normal)));
}
