#pragma once
#include "../Interfaces/IRenderer.h"
#include "GLContext.h"
#include "../Skybox.h"
#include "GLFramebuffer.h"

#include <random>
#include <unordered_map>

/***********************************************************************************/
// Forward Declarations
class Camera;
class Terrain;
struct RenderData;

namespace pugi {
	class xml_node;
}

/***********************************************************************************/
struct VisibleLightIndex {
	int Index;
};

/***********************************************************************************/
class GLRenderer : public IRenderer {

public:
	GLRenderer(const size_t width, const size_t height, const pugi::xml_node& rendererNode);

	void InitView(const Camera& camera);
	void Update(const Camera& camera, const double delta);
	void Shutdown() const;

	void Render(const Camera& camera, const RenderData& renderData);

private:
	void renderTerrain(const RenderData& renderData, const glm::vec3& cameraPos, const bool shadowPass);
	void renderModels(GLShaderProgram& shader, const RenderData& renderData, const bool shadowPass) const;
	// Render NDC screenquad
	void renderQuad() const;

	// Configure NDC screenquad
	void setupScreenquad();
	// Generate storage buffers for Forward+ rendering
	void setupLightBuffers();
	// Allocate space for lights
	void setupLightStorageBuffer();
	// Configure Depth FBO
	void setupDepthBuffer();
	// Configure HDR FBO
	void setupHDRBuffer();
	
	glm::vec3 RandomPosition(std::uniform_real_distribution<> dis, std::mt19937_64 gen);
	void UpdateLights(const double dt);

	// OpenGL context.
	GLContext m_context;

	// Screen dimensions
	size_t m_width, m_height;

	GLuint m_uboMatrices;

	// Projection matrix
	glm::mat4 m_projMatrix;

	// Storage buffers for lights and visible light indices
	GLuint m_lightBuffer, m_visibleLightIndicesBuffer;
	// Workgroup dimensions for compute shader
	GLuint m_workGroupsX, m_workGroupsY;

	// Depth pass
	GLuint m_depthTexture;
	GLFramebuffer m_depthFBO;

	// Environment map
	Skybox m_skybox;

	// Compiled shader cache
	std::unordered_map<std::string, GLShaderProgram> m_shaderCache;

	// Screen-quad
	GLuint m_quadVAO, m_quadVBO;

	// Post-Processing
	// HDR
	GLuint m_hdrColorBufferTexture;
	GLFramebuffer m_hdrFBO;

	// Saturation
	float m_saturation = 1.1f;

	// Vibrance
	float m_vibrance = 0.3f;
	const glm::vec4 m_coefficient{ 0.299f, 0.587f, 0.114f, 0.0f };


	const glm::vec3 LIGHT_MIN_BOUNDS = glm::vec3(-135.0f, -20.0f, -60.0f);
	const glm::vec3 LIGHT_MAX_BOUNDS = glm::vec3(135.0f, 170.0f, 60.0f);
	const float LIGHT_DELTA_TIME = -0.6f;
};
