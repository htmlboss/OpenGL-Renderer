#pragma once

#include <glad/glad.h>
#include "../Graphics/GLVertexArray.h"

#include <glm/vec4.hpp>

#include <vector>

//https://github.com/Tomius/LoD/tree/master/src/cpp/engine/cdlod

// A two-dimensional vector of GLshort values
struct svec2 {
	svec2() : x(0), y(0) {}
	svec2(const GLshort a, const GLshort b) : x(a), y(b) {}
	
	auto operator+(const svec2 rhs) const { return svec2(x + rhs.x, y + rhs.y); }
	
	friend svec2 operator*(GLshort lhs, const svec2 rhs) {
		return svec2(lhs * rhs.x, lhs * rhs.y);
	}

	GLshort x, y;
};

/***********************************************************************************/
class GridMesh {
	friend class QuadGridMesh;
public:
	GridMesh(const int dimension);

	void SetupPositions();
	void SetupRenderData();

	// xy: offset, z: scale, w: level
	void AddToRenderList(const glm::vec4& render_data);
	void ClearRenderList();

	auto GetDimension() const noexcept {
		return m_dimension;
	}

private:

	GLushort IndexOf(int x, int y);

	const int m_dimension;
	unsigned int m_indexCount;
	GLVertexArray m_vao;
	std::vector<glm::vec4> m_renderData; // xy: offset, z: scale, w: level
};

