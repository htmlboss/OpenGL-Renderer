#pragma once
#include <vector>

#include "Timer.h"
#include "Light.h"

#define KEY_PRESSED(KEY) (IRenderer::m_keys.at(KEY) = true, (void)0 )
#define KEY_RELEASED(KEY) (IRenderer::m_keys.at(KEY) = false, (void)0 )

class IRenderer {
	using Vec3 = std::array<float, 3>;
public:
	IRenderer(const size_t width, const size_t height) : m_width(width), m_height(height) {}
	virtual ~IRenderer() {}

	virtual void ClearColor(const float r = 0.0f, const float g = 0.0f, const float b = 0.0f, const float a = 1.0f) const = 0;
	virtual void Resize(const size_t width, const size_t height) = 0;
	virtual void GetDepthBuffer() const = 0;
	virtual void EnableBlending() const = 0;
	virtual void Render() const = 0;

	virtual void Update(const double glfwTimer) = 0;
	
	// Deferred Stuff
	virtual void BeginGeometryPass() const = 0;
	virtual void EndGeometryPass() const = 0;
	virtual void BeginLightingPass() const = 0;

	static void AddLight(const Vec3 Position, const Vec3 Color, const Light::LightType Type) { m_lights.emplace_back(Position, Color, Type); }

	static std::array<bool, 1024> m_keys;

protected:
	size_t m_width, m_height;
	Timer m_timer;

	static std::vector<Light> m_lights;

	const std::array<float, 20> m_screenQuadVertices {
		// Positions			// Texture Coords
		-1.0f, 1.0f, 0.0f,		0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,		1.0f, 0.0f
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

