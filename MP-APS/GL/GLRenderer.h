#pragma once
#include "../Interfaces/IRenderer.h"
#include "GLContext.h"

#include "GBuffer.h"
#include "../Skybox.h"
#include "GLPostProcess.h"

class Camera;
class Terrain;
struct RenderData;


class GLRenderer : public IRenderer {

public:
	GLRenderer(const size_t width, const size_t height);

	void InitView(const Camera& camera);
	void Update(const double deltaTime, const Camera& camera);
	void Shutdown() const;

	void GetDepthBuffer() const;

	void Render(const Camera& camera, const RenderData& renderData);

	// Deferred
	void DoGeometryPass() override;
	void DoDeferredLighting() const override;

private:
	void renderQuad() const;
	void createShadowDepthMap(const size_t shadowResolution);
	
	GLContext m_context;

	size_t m_width, m_height;

	GLuint m_uboMatrices;
	glm::mat4 m_projMatrix;

	GLFramebuffer m_depthMapFBO;
	GLuint m_depthMap;
	GLPostProcess m_postProcess;
	std::unique_ptr<GBuffer> m_gBuffer;

	Skybox m_skybox;

	GLShaderProgram m_forwardShader;
	GLShaderProgram m_skyboxShader;
	GLShaderProgram m_terrainShader;
	GLShaderProgram m_depthShader;

	// Screen-quad
	GLuint m_quadVAO, m_quadVBO;
};
