#pragma once

#include "../Skybox.h"
#include "../Model.h"
#include "../Graphics/GLFramebuffer.h"
#include "../Graphics/GLVertexArray.h"
#include "../Graphics/GLShaderProgram.h"

#include <unordered_map>
#include <vector>

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
	using RenderListIterator = std::vector<ModelPtr>::const_iterator;
public:

	void Init(const pugi::xml_node& rendererNode);
	void Update(const Camera& camera);
	// Release OpenGL resources
	void Shutdown() const;

	//
	void UpdateView(const Camera& camera);

	// Where the magic happens
	void Render(const Camera& camera,
				RenderListIterator renderListBegin,
				RenderListIterator renderListEnd,
				const SceneBase& scene,
				const bool globalWireframe = false
				);

private:
	// Helper functions

	
	// Sets the default state required for rendering
	void setDefaultState();
	// Render models contained in the renderlist
	void renderModelsWithTextures(GLShaderProgram& shader, RenderListIterator renderListBegin, RenderListIterator renderListEnd) const;
	// Render models without binding textures (for a depth or shadow pass perhaps)
	void renderModelsNoTextures(GLShaderProgram& shader, RenderListIterator renderListBegin, RenderListIterator renderListEnd) const;
	// Render NDC screenquad
	void renderQuad() const;
	// Renders shadowmap
	void renderShadowMap(const SceneBase& scene, RenderListIterator renderListBegin, RenderListIterator renderListEnd);
	// Configure NDC screenquad
	void setupScreenquad();
	// Setup texture samplers
	void setupTextureSamplers();
	// Setup FBO, resolution, etc for shadow mapping
	void setupShadowMap();
	// Configure post-processing effects
	void setupPostProcessing();

	// Screen dimensions
	std::size_t m_width{ 0 }, m_height{ 0 };

	// Uniform buffer for projection and view matrix
	GLuint m_uboMatrices{ 0 };

	// Projection matrix
	glm::mat4 m_projMatrix, m_lightSpaceMatrix;

	// Texture samplers
	GLuint m_samplerPBRTextures{ 0 };

	// Shadow mapping
	GLuint m_shadowMapResolution{ 1024 }, m_shadowDepthTexture{ 0 };
	GLFramebuffer m_shadowDepthFBO;

	// Environment map
	Skybox m_skybox;

	// Compiled shader cache
	std::unordered_map<std::string, GLShaderProgram> m_shaderCache;

	// Screen-quad
	GLVertexArray m_quadVAO;

	// Post-Processing
	// HDR
	GLuint m_hdrColorBuffer{ 0 }, m_brightnessThresholdColorBuffer{ 0 };
	GLFramebuffer m_hdrFBO;
	// Bloom
	std::array<GLFramebuffer, 2> m_pingPongFBOs;
	std::array<GLuint, 2> m_pingPongColorBuffers{0, 0};
	// Vibrance
	const float m_vibrance{ 0.1f };
	const glm::vec4 m_coefficient{ 0.299f, 0.587f, 0.114f, 0.0f };

};
