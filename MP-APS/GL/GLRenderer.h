#pragma once
#include <glad/glad.h>

#include "../Interfaces/IRenderer.h"

#include "GBuffer.h"
#include "GLPostProcess.h"
#include "../Camera.h"
#include "../Terrain.h"

#include <vector>

class Skybox;

class GLRenderer : public IRenderer {

public:
	GLRenderer(const std::size_t width, const std::size_t height);

	void Update(const double deltaTime) override;
	void Shutdown();

	void AddModels(const std::shared_ptr<Model>& model);

	void ClearColor(const float r = 0.0f, const float g = 0.0f, const float b = 0.0f, const float a = 1.0f) const override;
	
	void GetDepthBuffer() const;
	
	void Render() override;

	auto GetCameraPos() const noexcept { return m_camera->GetPosition(); }

	// Deferred
	void DoGeometryPass() override;
	void DoDeferredLighting() const override;

private:
	void renderQuad() const;
	void renderGeometry();

	// Store all loaded models
	std::vector<std::shared_ptr<Model>> m_models;

	GLuint m_uboMatrices;
	glm::mat4 m_projMatrix;

	std::unique_ptr<Camera> m_camera;
	std::unique_ptr<GBuffer> m_gBuffer;

	std::unique_ptr<Skybox> m_skybox;
	std::unique_ptr<Terrain> m_terrain1;
	std::unique_ptr<GLPostProcess> m_postProcess;

	GLShaderProgramPtr m_forwardShader;
	GLShaderProgramPtr m_skyboxShader;
	GLShaderProgramPtr m_terrainShader;
	GLShaderProgramPtr m_PBRShader;

	// Screen-quad
	GLuint m_quadVAO, m_quadVBO;
};
