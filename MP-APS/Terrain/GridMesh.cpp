#include "GridMesh.h"

/***********************************************************************************/
GridMesh::GridMesh(const int dimension) : m_dimension(dimension) {
}

/***********************************************************************************/
void GridMesh::SetupPositions() {
	std::vector<svec2> positions;
	positions.reserve((m_dimension + 1) * (m_dimension + 1));

	const auto dim_2 = m_dimension / 2;

	for (auto y = -dim_2; y <= dim_2; ++y) {
		for (auto x = -dim_2; x <= dim_2; ++x) {
			positions.emplace_back(svec2(x, y));
		}
	}

	std::vector<GLushort> indices;
	m_indexCount = 2 * (m_dimension + 1)*(m_dimension) + 2 * m_dimension;
	indices.reserve(m_indexCount);

	for (auto y = -dim_2; y < dim_2; ++y) {
		for (auto x = -dim_2; x <= dim_2; ++x) {
			indices.push_back(IndexOf(x, y));
			indices.push_back(IndexOf(x, y + 1));
		}

		// Create a degenerate (as primitive restart)
		indices.push_back(IndexOf(dim_2, y + 1));
		indices.push_back(IndexOf(-dim_2, y + 1));
	}

	m_vao.Init();
	m_vao.Bind();
	
	m_vao.AttachBuffer(GLVertexArray::BufferType::ARRAY, positions.size() * sizeof(svec2), GLVertexArray::DrawMode::STATIC, &positions[0]);
	m_vao.AttachBuffer(GLVertexArray::BufferType::ELEMENT, indices.size() * sizeof(GLushort), GLVertexArray::DrawMode::STATIC, &indices[0]);

	m_vao.EnableAttribute(0, 2, sizeof(svec2), nullptr); // This could be wrong?
}

/***********************************************************************************/
void GridMesh::SetupRenderData() {
	m_vao.Bind();

}

/***********************************************************************************/
void GridMesh::AddToRenderList(const glm::vec4& render_data) {
	m_renderData.push_back(render_data);
}

/***********************************************************************************/
void GridMesh::ClearRenderList() {
	m_renderData.erase(m_renderData.begin(), m_renderData.end());
}

/***********************************************************************************/
GLushort GridMesh::IndexOf(int x,int y) {
	x += m_dimension / 2;
	y += m_dimension / 2;
	return (m_dimension + 1) * y + x;
}

/***********************************************************************************/
