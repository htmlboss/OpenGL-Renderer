#include "Mesh.h"
#include "Utils/Utils.h"

#include <string>

/***********************************************************************************/
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) : m_indexCount{ indices.size() }, m_vao{} {
	
	setupMesh(vertices, indices);
}

/***********************************************************************************/
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const std::vector<GLTexture>& textures) :
	m_vao{},
	m_indexCount{indices.size()},
	m_textures(textures) {

	setupMesh(vertices, indices);
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
void Mesh::setupMesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) {
	m_vao.Bind();
	// Attach VBO
	m_vao.AttachBuffer(GLVertexArray::BufferType::ARRAY, vertices.size() * sizeof(Vertex), GLVertexArray::DrawMode::STATIC, &vertices[0]);
	// Attach EBO
	m_vao.AttachBuffer(GLVertexArray::BufferType::ELEMENT, indices.size() * sizeof(GLuint), GLVertexArray::DrawMode::STATIC, &indices[0]);

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