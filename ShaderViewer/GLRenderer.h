#pragma once
#include "IRenderer.h"
#include "GBuffer.h"
#include "Camera.h"

#include <memory>


class GLRenderer : public IRenderer {
public:
	GLRenderer(const size_t width, const size_t height);
	~GLRenderer();

	void ClearColor(const float r = 0.0f, const float g = 0.0f, const float b = 0.0f, const float a = 0.0f) const override;
	void Resize(const size_t width, const size_t height) override;
	void GetDepthBuffer() const override;
	void EnableBlending() const override;
	void RenderScreenQuad() const override;
	static void UpdateMouse(const double xPos, const double yPos);

	glm::vec3 GetCameraPos() const { return m_camera.GetPosition(); }
	void Update(const double glfwTimer) override;

	// Deferred
	void BeginGeometryPass() const override;
	void EndGeometryPass() const override;

	void BeginLightingPass() const override;

//private:
	std::unique_ptr<GBuffer> m_gBuffer;
private:
	static Camera m_camera;
	GLuint m_uboMatrices;
	
	// Mouse stuff
	static bool m_firstMouse;
	static double m_prevX, m_prevY;
};
