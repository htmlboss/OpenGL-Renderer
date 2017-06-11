#pragma once
#include "../Interfaces/IRenderer.h"
#include "GLContext.h"

#include "GBuffer.h"
#include "../Skybox.h"
#include "GLPostProcess.h"
#include "../Model.h"

class Camera;
class Terrain;


class GLRenderer : public IRenderer {

public:
	GLRenderer(const size_t width, const size_t height);

	void InitView(const Camera& camera);
	void Update(const double deltaTime, const Camera& camera);
	void Shutdown() const;

	void GetDepthBuffer() const;

	void Render(const Camera& camera, Terrain& terrain, const std::vector<ModelPtr>& renderList);

	// Deferred
	void DoGeometryPass() override;
	void DoDeferredLighting() const override;

private:
	void renderQuad() const;
	
	GLContext m_context;

	size_t m_width, m_height;

	GLuint m_uboMatrices;
	glm::mat4 m_projMatrix;

	std::unique_ptr<GBuffer> m_gBuffer;

	Skybox m_skybox;
	std::unique_ptr<GLPostProcess> m_postProcess;

	GLShaderProgram m_forwardShader;
	GLShaderProgram m_skyboxShader;
	GLShaderProgram m_terrainShader;

	// Screen-quad
	GLuint m_quadVAO, m_quadVBO;
};
