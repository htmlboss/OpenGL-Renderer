#pragma once
#include "../Interfaces/IRenderer.h"
#include "GLContext.h"

#include "../Skybox.h"
#include "GLPostProcess.h"

/***********************************************************************************/
// Forward Declarations
class Camera;
class Terrain;
struct RenderData;

/***********************************************************************************/
struct VisibleLightIndex {
	int Index;
};

/***********************************************************************************/
class GLRenderer : public IRenderer {

public:
	GLRenderer(const size_t width, const size_t height);

	void InitView(const Camera& camera);
	void Update(const Camera& camera);
	void Shutdown() const;

	void Render(const Camera& camera, const RenderData& renderData);

private:
	void renderTerrain(const RenderData& renderData, const glm::vec3& cameraPos, const bool shadowPass);
	void renderModels(GLShaderProgram& shader, const RenderData& renderData, const bool shadowPass) const;
	void renderQuad() const;

	// Configure NDC screenquad
	void setupScreenquad();
	// Generate storage buffers for Forward+ rendering
	void setupLightBuffers();
	// Allocate space for lights
	void setupLightStorageBuffer();
	// Configure Depth FBO
	void setupDepthBuffer() const;
	
	GLContext m_context;

	size_t m_width, m_height;

	GLuint m_uboMatrices;
	glm::mat4 m_projMatrix;

	// Storage buffers for lights and visiable light indices
	GLuint m_lightBuffer, m_visibleLightIndicesBuffer;
	// Workgroup dimensions for compute shader
	GLuint m_workGroupsX, m_workGroupsY;

	GLPostProcess m_postProcess;
	GLFramebuffer m_depthFBO;

	Skybox m_skybox;

	// Shaders
	GLShaderProgram m_forwardShader;
	GLShaderProgram m_skyboxShader;
	GLShaderProgram m_terrainShader;
	GLShaderProgram m_depthShader;
	GLShaderProgram m_depthDebugShader;

	// Screen-quad
	GLuint m_quadVAO, m_quadVBO;
};
