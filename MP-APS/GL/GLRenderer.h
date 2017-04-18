#pragma once
#include "glad/glad.h"

#include "../Interfaces/IRenderer.h"

#include "GLShaderProgram.h"
#include "GBuffer.h"
#include "../Camera.h"
#include "../Model.h"

#include <memory>
#include <vector>

class Skybox;

class GLRenderer : public IRenderer {

public:
	GLRenderer(const std::size_t width, const std::size_t height);
	~GLRenderer();

	void Update(const double deltaTime) override;
	void Shutdown();

	void AddModels(const std::shared_ptr<Model>& model);

	void ClearColor(const float r = 0.0f, const float g = 0.0f, const float b = 0.0f, const float a = 1.0f) const override;
	
	void GetDepthBuffer() const override;
	void EnableBlending() const override;
	
	void Render() const override;
	void RenderSkybox(GLShaderProgram& shader) const;

	static glm::vec3 GetCameraPos() { return m_camera->GetPosition(); }

	static void Resize(const std::size_t width, const std::size_t height);

	// Update view from mouse movement
	static void UpdateCameraView(double xPos, double yPos);

	// Deferred
	void DoGeometryPass() override;
	void DoDeferredLighting() const override;

private:
	static std::size_t m_width, m_height;
	
	std::vector<std::shared_ptr<Model>> m_models;

	GLuint m_uboMatrices;
	glm::mat4 m_projMatrix;

	static std::unique_ptr<Camera> m_camera;
	std::unique_ptr<GBuffer> m_gBuffer;
	std::unique_ptr<Skybox> m_skybox;
	std::unique_ptr<GLShaderProgram> m_shaderSSAO;

	static bool m_shouldResize;

	// Screen-quad
	GLuint m_quadVAO, m_quadVBO;

	// Mouse stuff
	static bool m_firstMouse;
	static double m_prevX, m_prevY;
};
