#pragma once

#include "../Skybox.h"
#include "../Model.h"
#include "../Graphics/GLFramebuffer.h"
#include "../Graphics/GLVertexArray.h"
#include "../Graphics/GLShaderProgram.h"

#include <unordered_map>

/***********************************************************************************/
// Forward Declarations
class Camera;
class SceneBase;
class GLShaderProgram;
namespace pugi {
	class xml_node;
}

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
	// Renders shadowmap
	void renderShadowMap(const SceneBase& scene);
	// Configure NDC screenquad
	void setupScreenquad();
	// Setup texture samplers
	void setupTextureSamplers();
	// Setup FBO, resolution, etc for shadow mapping
	void setupShadowMap();
	// Configure post-processing effects
	void setupPostProcessing();

	// Screen dimensions
	std::size_t m_width, m_height;

	// Uniform buffer for projection and view matrix
	GLuint m_uboMatrices;

	// Projection matrix
	glm::mat4 m_projMatrix, m_lightSpaceMatrix;

	// Texture samplers
	GLuint m_samplerPBRTextures;

	// Shadow mapping
	GLuint m_shadowMapResolution, m_shadowDepthTexture;
	GLFramebuffer m_shadowDepthFBO;

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

};
