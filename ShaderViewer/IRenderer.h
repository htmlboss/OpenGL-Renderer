#pragma once
#include <array>

#include "Timer.h"

#define KEY_PRESSED(KEY) (IRenderer::m_keys.at(KEY) = true, (void)0 )
#define KEY_RELEASED(KEY) (IRenderer::m_keys.at(KEY) = false, (void)0 )

class IRenderer {

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

	static std::array<bool, 1024> m_keys;

protected:
	size_t m_width, m_height;

	Timer m_timer;

	const std::array<float, 20> m_screenQuadVertices = {
		// Positions			// Texture Coords
		-1.0f, 1.0f, 0.0f,		0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f,		0.0f, 0.0f,
		1.0f, 1.0f, 0.0f,		1.0f, 1.0f,
		1.0f, -1.0f, 0.0f,		1.0f, 0.0f
	};
};

