#pragma once

#include "../Skybox.h"
#include "../Model.h"
#include "../Graphics/GLFramebuffer.h"
#include "../Graphics/GLVertexArray.h"

#include <unordered_map>
#include <random>

/***********************************************************************************/
// Forward Declarations
class Terrain;
class Camera;
class SceneBase;
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
	void renderModels(GLShaderProgram& shader, const std::vector<ModelPtr>& renderList, const bool shadowPass) const;
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

	// Screen dimensions
	std::size_t m_width, m_height;

	// Uniform buffer for projection and view matrix
	GLuint m_uboMatrices;

	// Projection matrix
	glm::mat4 m_projMatrix;

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
	GLuint m_hdrColorBufferTexture;
	GLFramebuffer m_hdrFBO;
	// Saturation
	float m_saturation = 1.1f;
	// Vibrance
	float m_vibrance = 0.3f;
	const glm::vec4 m_coefficient{ 0.299f, 0.587f, 0.114f, 0.0f };

	const std::size_t MAX_NUM_LIGHTS = 128;
	const glm::vec3 LIGHT_MIN_BOUNDS = glm::vec3(-135.0f, -20.0f, -60.0f);
	const glm::vec3 LIGHT_MAX_BOUNDS = glm::vec3(135.0f, 170.0f, 60.0f);
	const float LIGHT_DELTA_TIME = -0.6f;
};
