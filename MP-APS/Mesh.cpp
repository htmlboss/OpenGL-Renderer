#include "Mesh.h"
#include "Utils/Utils.h"

#include <string>
#include <stddef.h>

/***********************************************************************************/
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) : m_renderData(indices), m_vertices(vertices) {
	setupMesh();
}

/***********************************************************************************/
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<GLTexture>& textures) :
	m_renderData(indices),
	m_vertices(vertices),
	m_textures(textures) {

	//Construct mesh
	setupMesh();
}

/***********************************************************************************/
void Mesh::BindTextures(GLShaderProgram* shader) {
	using namespace Utils;

	GLuint albedoNr = 1, specularNr = 1, normalNr = 1;

	GLuint index = 0;

	for (const auto& it : m_textures) {
		glActiveTexture(GL_TEXTURE0 + index); // Activate proper texture unit before binding
		// Retrieve texture number (the N in diffuse_textureN)

		const auto name = it.GetSampler();
		const auto textureNameHashed = str2int(name.c_str());
		std::string number;

		switch (textureNameHashed) {
			case str2int("texture_diffuse"):
				number = std::to_string(albedoNr++);
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
}

/***********************************************************************************/
void Mesh::setupMesh() {

	m_renderData.VAO.Bind();
	// Attach VBO
	m_renderData.VAO.AttachBuffer(GLVertexArray::BufferType::ARRAY, m_vertices.size() * sizeof(Vertex), GLVertexArray::DrawMode::STATIC, &m_vertices[0]);
	// Attach EBO
	m_renderData.VAO.AttachBuffer(GLVertexArray::BufferType::ELEMENT, m_renderData.Indices.size() * sizeof(GLuint), GLVertexArray::DrawMode::STATIC, &m_renderData.Indices[0]);

	// Vertex Attributes

	const auto vertexSize = sizeof(Vertex);
	// Position
	m_renderData.VAO.EnableAttribute(0, 3, vertexSize, nullptr);
	// Texture Coords
	m_renderData.VAO.EnableAttribute(1, 2, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
	// Normal
	m_renderData.VAO.EnableAttribute(2, 3, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	// Tangent
	m_renderData.VAO.EnableAttribute(3, 3, vertexSize, reinterpret_cast<GLvoid*>(offsetof(Vertex, Tangent)));
	// Bitangent
	m_renderData.VAO.EnableAttribute(4, 3, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, Bitangent)));


	m_vertices.clear();
	m_vertices.shrink_to_fit();
}
