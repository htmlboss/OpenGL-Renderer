#pragma once
#include "../Interfaces/IRenderer.h"
#include "GLContext.h"

#include "../Skybox.h"
#include "GLPostProcess.h"
#include <random>

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
	void Update(const Camera& camera, const double delta);
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
	void setupDepthBuffer();
	
	glm::vec3 RandomPosition(std::uniform_real_distribution<> dis, std::mt19937_64 gen);
	void UpdateLights(const double dt);

	// OpenGL context.
	GLContext m_context;

	// Screen dimensions
	size_t m_width, m_height;

	GLuint m_uboMatrices;

	// Projection matrix
	glm::mat4 m_projMatrix;

	// Storage buffers for lights and visiable light indices
	GLuint m_lightBuffer, m_visibleLightIndicesBuffer;
	// Workgroup dimensions for compute shader
	GLuint m_workGroupsX, m_workGroupsY;

	GLPostProcess m_postProcess;

	// Depth pass
	GLuint m_depthTexture;
	GLFramebuffer m_depthFBO;

	Skybox m_skybox;

	// Shaders
	GLShaderProgram m_skyboxShader;
	GLShaderProgram m_terrainShader;
	GLShaderProgram m_depthShader;
	GLShaderProgram m_depthDebugShader;
	GLShaderProgram m_lightCullingShader;
	GLShaderProgram m_lightAccumShader;

	// Screen-quad
	GLuint m_quadVAO, m_quadVBO;

	const glm::vec3 LIGHT_MIN_BOUNDS = glm::vec3(-135.0f, -20.0f, -60.0f);
	const glm::vec3 LIGHT_MAX_BOUNDS = glm::vec3(135.0f, 170.0f, 60.0f);
	const float LIGHT_DELTA_TIME = -0.6f;
};
