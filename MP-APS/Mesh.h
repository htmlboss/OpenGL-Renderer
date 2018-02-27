#pragma once

#include "Vertex.h"
#include "Graphics/GLVertexArray.h"
#include "PBRMaterial.h"

#include <vector>

// TODO: Add per-mesh AABB
/***********************************************************************************/
struct Mesh {
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
	Mesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices, const PBRMaterialPtr& material);

	void Clear();

	auto GetTriangleCount() const noexcept { return IndexCount / 3; }
	
	const std::size_t IndexCount;
	GLVertexArray VAO;
	PBRMaterialPtr Material;

private:
	void setupMesh(const std::vector<Vertex>& vertices, const std::vector<GLuint>& indices);
};
