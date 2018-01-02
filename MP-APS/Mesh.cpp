#include "Mesh.h"

/***********************************************************************************/
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) : IndexCount(indices.size()) {

	setupMesh(vertices, indices);
}

/***********************************************************************************/
Mesh::Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const PBRMaterial& material) :
	IndexCount(indices.size()),
	Material(material) {

	setupMesh(vertices, indices);
}

/***********************************************************************************/
void Mesh::setupMesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices) {
	
	VAO.Init();
	VAO.Bind();
	// Attach VBO
	VAO.AttachBuffer(GLVertexArray::BufferType::ARRAY, vertices.size() * sizeof(Vertex), GLVertexArray::DrawMode::STATIC, &vertices[0]);
	// Attach EBO
	VAO.AttachBuffer(GLVertexArray::BufferType::ELEMENT, indices.size() * sizeof(GLuint), GLVertexArray::DrawMode::STATIC, &indices[0]);

	// Vertex Attributes

	const static auto vertexSize = sizeof(Vertex);
	// Position
	VAO.EnableAttribute(0, 3, vertexSize, nullptr);
	// Texture Coords
	VAO.EnableAttribute(1, 2, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
	// Normal
	VAO.EnableAttribute(2, 3, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, Normal)));
	// Tangent
	VAO.EnableAttribute(3, 3, vertexSize, reinterpret_cast<void*>(offsetof(Vertex, Tangent)));
}