#pragma once

#include "../Skybox.h"
#include "../Model.h"
#include "../Graphics/GLFramebuffer.h"
#include "../Graphics/GLVertexArray.h"
#include "../Graphics/GLShaderProgram.h"

#include <unordered_map>
#include <random>

/***********************************************************************************/
// Forward Declarations
class Terrain;
class Camera;
class SceneBase;
class GLShaderProgram;
struct RenderData;
namespace pugi {
	class xml_node;
}

/***********************************************************************************/
struct VisibleLightIndex {
	int Index;
};

/***********************************************************************************/
class RenderSystem {

public:
	RenderSystem();

	RenderSystem(const RenderSystem&) = delete;
	RenderSystem& operator=(const RenderSystem&) = delete;

	void Init(const pugi::xml_node& rendererNode);
	void Update(const Camera& camera, const double delta);
	// Release OpenGL resources
	void Shutdown() const;

	void InitView(const Camera& camera);

	// Where the magic happens
	void Render(const SceneBase& scene, const bool wireframe = false);

private:
	// Helper functions

	// Render models contained in the renderlist
	void renderModelsWithTextures(GLShaderProgram& shader, const std::vector<ModelPtr>& renderList) const;
	// Render models without binding textures (for a depth or shadow pass perhaps)
	void renderModelsNoTextures(GLShaderProgram& shader, const std::vector<ModelPtr>& renderList) const;
	// Render NDC screenquad
	void renderQuad() const;
	// Configure NDC screenquad
	void setupScreenquad();
	// Generate storage buffers for Forward+ rendering
	void setupLightBuffers();
	// Setup texture samplers
	void setupTextureSamplers();
	// Allocate space for lights
	void setupLightStorageBuffer();
	// Configure Depth FBO
	void setupDepthBuffer();
	// Configure post-processing effects
	void setupPostProcessing();
	
	glm::vec3 RandomPosition(std::uniform_real_distribution<> dis, std::mt19937_64 gen);
	void UpdateLights(const double dt);

	// Screen dimensions
	std::size_t m_width, m_height;

	// Uniform buffer for projection and view matrix
	GLuint m_uboMatrices;

	// Projection matrix
	glm::mat4 m_projMatrix;

	// Texture samplers
	GLuint m_samplerPBRTextures;

	// Storage buffers for lights and visible light indices
	GLuint m_lightBuffer, m_visibleLightIndicesBuffer;
	// Workgroup dimensions for light culling compute shader
	GLuint m_workGroupsX, m_workGroupsY;

	// Depth pass
	GLuint m_depthTexture;
	GLFramebuffer m_depthFBO;

	// Environment map
	Skybox m_skybox;

	// Compiled shader cache
	std::unordered_map<std::string, GLShaderProgram> m_shaderCache;

	// Screen-quad
	GLVertexArray m_quadVAO;

	// Post-Processing
	// HDR
	GLuint m_hdrColorBuffer, m_brightnessThresholdColorBuffer;
	GLFramebuffer m_hdrFBO;
	// Bloom
	std::array<GLFramebuffer, 2> m_pingPongFBOs;
	std::array<GLuint, 2> m_pingPongColorBuffers;
	// Vibrance
	const float m_vibrance = 0.1f;
	const glm::vec4 m_coefficient{ 0.299f, 0.587f, 0.114f, 0.0f };

	const std::size_t MAX_NUM_LIGHTS = 128;
	const glm::vec3 LIGHT_MIN_BOUNDS = glm::vec3(-135.0f, -20.0f, -60.0f);
	const glm::vec3 LIGHT_MAX_BOUNDS = glm::vec3(135.0f, 170.0f, 60.0f);
	const float LIGHT_DELTA_TIME = -0.6f;
};
