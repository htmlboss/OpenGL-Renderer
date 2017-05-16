#pragma once

#include "../Vertex.h"

#include <array>

#define KEY_PRESSED(KEY) (IRenderer::m_keys.at(KEY) = true, (void)0 )
#define KEY_RELEASED(KEY) (IRenderer::m_keys.at(KEY) = false, (void)0 )

class IRenderer {
	using Vec3 = std::array<float, 3>;
public:
	IRenderer() noexcept {};
	virtual ~IRenderer() = default;

	virtual void ClearColor(const float r = 0.0f, const float g = 0.0f, const float b = 0.0f, const float a = 1.0f) const = 0;
	virtual void Render() = 0;

	virtual void Update(const double delta) = 0;
	
	// Deferred Stuff
	virtual void DoGeometryPass() = 0;
	virtual void DoDeferredLighting() const = 0;

	static std::array<bool, 1024> m_keys;

protected:

	const std::array<Vertex, 4> m_screenQuadVertices {
				// Positions			// GLTexture Coords
		Vertex({-1.0f, 1.0f, 0.0f},		{0.0f, 1.0f}),
		Vertex({-1.0f, -1.0f, 0.0f},	{0.0f, 0.0f}),
		Vertex({1.0f, 1.0f, 0.0f},		{1.0f, 1.0f}),
		Vertex({1.0f, -1.0f, 0.0f},		{1.0f, 0.0f})
	};

	const std::array<float, 108> m_cubeVertices {
		// Back face
		-0.5f, -0.5f, -0.5f,  // Bottom-left
		 0.5f,  0.5f, -0.5f,  // top-right
		 0.5f, -0.5f, -0.5f,  // bottom-right         
		 0.5f,  0.5f, -0.5f,  // top-right
		-0.5f, -0.5f, -0.5f,  // bottom-left
		-0.5f,  0.5f, -0.5f,  // top-left
		// Front face
		-0.5f, -0.5f,  0.5f,  // bottom-left
		 0.5f, -0.5f,  0.5f,  // bottom-right
		 0.5f,  0.5f,  0.5f,  // top-right
		 0.5f,  0.5f,  0.5f,  // top-right
		-0.5f,  0.5f,  0.5f,  // top-left
		-0.5f, -0.5f,  0.5f,  // bottom-left
		// Left face
		-0.5f,  0.5f,  0.5f,  // top-right
		-0.5f,  0.5f, -0.5f,  // top-left
		-0.5f, -0.5f, -0.5f,  // bottom-left
		-0.5f, -0.5f, -0.5f,  // bottom-left
		-0.5f, -0.5f,  0.5f,  // bottom-right
		-0.5f,  0.5f,  0.5f,  // top-right
		// Right face
		 0.5f,  0.5f,  0.5f, // top-left
		 0.5f, -0.5f, -0.5f, // bottom-right
		 0.5f,  0.5f, -0.5f, // top-right         
		 0.5f, -0.5f, -0.5f, // bottom-right
		 0.5f,  0.5f,  0.5f, // top-left
		 0.5f, -0.5f,  0.5f, // bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f, // top-right
		 0.5f, -0.5f, -0.5f, // top-left
		 0.5f, -0.5f,  0.5f, // bottom-left
		 0.5f, -0.5f,  0.5f, // bottom-left
		-0.5f, -0.5f,  0.5f, // bottom-right
		-0.5f, -0.5f, -0.5f, // top-right
		// Top face
		-0.5f,  0.5f, -0.5f, // top-left
		 0.5f,  0.5f,  0.5f, // bottom-right
		 0.5f,  0.5f, -0.5f, // top-right     
		 0.5f,  0.5f,  0.5f, // bottom-right
		-0.5f,  0.5f, -0.5f, // top-left
		-0.5f,  0.5f,  0.5f  // bottom-left        
	};
};

