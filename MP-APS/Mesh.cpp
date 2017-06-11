#include "Mesh.h"
#include "Utils/Utils.h"

#include <string>
#include <stddef.h>

/***********************************************************************************/
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) : m_vao(), m_vertices(vertices), m_indices(indices) { setupMesh(); }

/***********************************************************************************/
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<GLTexture>& textures) :
	m_uniformsAdded(false),
	m_vertices(vertices),
	m_indices(indices),
	m_textures(textures) {

	//Construct mesh
	setupMesh();
}

/***********************************************************************************/
void Mesh::SetInstancing(const std::initializer_list<glm::vec3>& args) {

	m_instanceOffsets = args;

	m_vao.Bind();
	m_vao.AttachBuffer(GLVertexArray::BufferType::ARRAY, sizeof(glm::vec3) * args.size(), GLVertexArray::DrawMode::STATIC, args.begin());
	m_vao.EnableAttribute(5, 3, 3 * sizeof(GLfloat), nullptr);
	glVertexAttribDivisor(5, 1);
	/*
	glBindVertexArray(m_vao);
	glGenBuffers(1, &m_instanceVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * args.size(), args.begin(), GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// Vertex Instance offset
	glEnableVertexAttribArray(5);
	glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glVertexAttribDivisor(5, 1); // Tell OpenGL this is an instanced vertex attribute.
	*/
}

/***********************************************************************************/
void Mesh::Draw(GLShaderProgram* shader) {

	bindTextures(shader);

	// Draw mesh
	m_vao.Bind();
	glDrawElements(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);
}

/***********************************************************************************/
void Mesh::DrawInstanced(GLShaderProgram* shader) {

	bindTextures(shader);

	// Draw instanced mesh
	m_vao.Bind();
	glDrawElementsInstanced(GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_INT, nullptr, m_instanceOffsets.size());
}

/***********************************************************************************/
void Mesh::bindTextures(GLShaderProgram* shader) {
	using namespace Utils;

	GLuint albedoNr = 1, normalNr = 1, metallicNr = 1, roughnessNr = 1, aoNr = 1;

	GLuint index = 0;

	for (const auto& it : m_textures) {
		glActiveTexture(GL_TEXTURE0 + index); // Activate proper texture unit before binding
		// Retrieve texture number (the N in diffuse_textureN)

		const auto name = it.GetSampler();
		const auto textureNameHashed = str2int(name.c_str());
		std::string number;

		switch (textureNameHashed) {
			case str2int("albedoMap"):
				number = std::to_string(albedoNr++);
				break;
			case str2int("normalMap"):
				number = std::to_string(normalNr++);
				break;
			case str2int("metallicMap"):
				number = std::to_string(metallicNr++);
				break;
			case str2int("roughnessMap"):
				number = std::to_string(roughnessNr++);
				break;
			case str2int("aoMap"):
				number = std::to_string(aoNr++);
				break;
		}

		shader->SetUniformi(name + number, index);
		glBindTexture(GL_TEXTURE_2D, it.GetTexture());

		++index;
	}
}

/***********************************************************************************/
void Mesh::setupMesh() {

	m_vao.Bind();
	// Attach VBO
	m_vao.AttachBuffer(GLVertexArray::BufferType::ARRAY, m_vertices.size() * sizeof(Vertex), GLVertexArray::DrawMode::STATIC, &m_vertices[0]);
	// Attach EBO
	m_vao.AttachBuffer(GLVertexArray::BufferType::ELEMENT, m_indices.size() * sizeof(GLuint), GLVertexArray::DrawMode::STATIC, &m_indices[0]);

	// Vertex Attributes

	const auto vertexSize = sizeof(Vertex);
	// Position
	m_vao.EnableAttribute(0, 3, vertexSize, nullptr);
	// Texture Coords
	m_vao.EnableAttribute(1, 2, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
	// Normal
	m_vao.EnableAttribute(2, 3, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	// Tangent
	m_vao.EnableAttribute(3, 3, vertexSize, reinterpret_cast<GLvoid*>(offsetof(Vertex, Tangent)));
	// Bitangent
	m_vao.EnableAttribute(4, 3, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, Bitangent)));
}
