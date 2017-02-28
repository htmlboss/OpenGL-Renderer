#pragma once
#include "IRenderer.h"
#include "GBuffer.h"
#include "Camera.h"

#include <memory>

class GLShaderProgram;
class SkySphere;
class Skybox;

class GLRenderer : public IRenderer {

public:
	GLRenderer(const size_t width, const size_t height);
	~GLRenderer();

	void ClearColor(const float r = 0.0f, const float g = 0.0f, const float b = 0.0f, const float a = 1.0f) const override;
	void Resize(const size_t width, const size_t height) override;
	void GetDepthBuffer() const override;
	void EnableBlending() const override;
	
	void Render() const override;
	void RenderSkybox(GLShaderProgram& shader) const;
	static void UpdateMouse(const double xPos, const double yPos);

	static glm::vec3 GetCameraPos() { return m_camera.GetPosition(); }
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

	std::unique_ptr<Skybox> m_skybox;
	//std::unique_ptr<SkySphere> m_skySphere;

	// Screen-quad
	GLuint m_quadVAO, m_quadVBO;
	
	// Mouse stuff
	static bool m_firstMouse;
	static double m_prevX, m_prevY;

	// Lights
	GLuint m_lightVAO, m_lightVBO;
};
