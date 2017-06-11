#pragma once

#include "../Vertex.h"

#include <array>

class IRenderer {
public:
	IRenderer() noexcept {
	};
	virtual ~IRenderer() = default;

	// Deferred Stuff
	virtual void DoGeometryPass() = 0;
	virtual void DoDeferredLighting() const = 0;

protected:

	const std::array<Vertex, 4> m_screenQuadVertices{
		// Positions			// GLTexture Coords
		Vertex({-1.0f, 1.0f, 0.0f}, {0.0f, 1.0f}),
		Vertex({-1.0f, -1.0f, 0.0f}, {0.0f, 0.0f}),
		Vertex({1.0f, 1.0f, 0.0f}, {1.0f, 1.0f}),
		Vertex({1.0f, -1.0f, 0.0f}, {1.0f, 0.0f})
	};

	const std::array<float, 108> m_cubeVertices{
		// Back face
		-0.5f, -0.5f, -0.5f, // Bottom-left
		0.5f, 0.5f, -0.5f, // top-right
		0.5f, -0.5f, -0.5f, // bottom-right         
		0.5f, 0.5f, -0.5f, // top-right
		-0.5f, -0.5f, -0.5f, // bottom-left
		-0.5f, 0.5f, -0.5f, // top-left
		// Front face
		-0.5f, -0.5f, 0.5f, // bottom-left
		0.5f, -0.5f, 0.5f, // bottom-right
		0.5f, 0.5f, 0.5f, // top-right
		0.5f, 0.5f, 0.5f, // top-right
		-0.5f, 0.5f, 0.5f, // top-left
		-0.5f, -0.5f, 0.5f, // bottom-left
		// Left face
		-0.5f, 0.5f, 0.5f, // top-right
		-0.5f, 0.5f, -0.5f, // top-left
		-0.5f, -0.5f, -0.5f, // bottom-left
		-0.5f, -0.5f, -0.5f, // bottom-left
		-0.5f, -0.5f, 0.5f, // bottom-right
		-0.5f, 0.5f, 0.5f, // top-right
		// Right face
		0.5f, 0.5f, 0.5f, // top-left
		0.5f, -0.5f, -0.5f, // bottom-right
		0.5f, 0.5f, -0.5f, // top-right         
		0.5f, -0.5f, -0.5f, // bottom-right
		0.5f, 0.5f, 0.5f, // top-left
		0.5f, -0.5f, 0.5f, // bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f, // top-right
		0.5f, -0.5f, -0.5f, // top-left
		0.5f, -0.5f, 0.5f, // bottom-left
		0.5f, -0.5f, 0.5f, // bottom-left
		-0.5f, -0.5f, 0.5f, // bottom-right
		-0.5f, -0.5f, -0.5f, // top-right
		// Top face
		-0.5f, 0.5f, -0.5f, // top-left
		0.5f, 0.5f, 0.5f, // bottom-right
		0.5f, 0.5f, -0.5f, // top-right     
		0.5f, 0.5f, 0.5f, // bottom-right
		-0.5f, 0.5f, -0.5f, // top-left
		-0.5f, 0.5f, 0.5f // bottom-left        
	};
};
