#pragma once
#include "../Interfaces/ISystem.h"

#include "../Skybox.h"
#include "../Vertex.h"
#include "GLFramebuffer.h"

#include <random>
#include <array>
#include <unordered_map>
#include <memory>

/***********************************************************************************/
// Forward Declarations
class Terrain;
struct RenderData;

/***********************************************************************************/
struct VisibleLightIndex {
	int Index;
};

/***********************************************************************************/
class GLRenderer : public ISystem {

public:
	GLRenderer();
	GLRenderer(const GLRenderer&) = delete;
	GLRenderer& operator=(const GLRenderer&) = delete;

	void OnInit(const pugi::xml_node& rendererNode) override;
	void OnUpdate(const Camera& camera, const double delta) override;

	void InitView(const Camera& camera);
	void OnSucceed() const override;

	void Render(const Camera& camera, const RenderData& renderData);

private:
	void OnAbort(const std::string_view error) override;

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

	// Screen dimensions
	std::size_t m_width, m_height;

	GLuint m_uboMatrices;

	// Projection matrix
	glm::mat4 m_projMatrix;

	// Storage buffers for lights and visible light indices
	GLuint m_lightBuffer, m_visibleLightIndicesBuffer;
	// Workgroup dimensions for compute shader
	GLuint m_workGroupsX, m_workGroupsY;

	// Depth pass
	GLuint m_depthTexture;
	std::unique_ptr<GLFramebuffer> m_depthFBO;

	// Environment map
	std::unique_ptr<Skybox> m_skybox;

	// Compiled shader cache
	std::unordered_map<std::string, GLShaderProgram> m_shaderCache;

	// Screen-quad
	GLuint m_quadVAO, m_quadVBO;

	// Post-Processing
	// HDR
	GLuint m_hdrColorBufferTexture;
	std::unique_ptr<GLFramebuffer> m_hdrFBO;

	// Saturation
	float m_saturation = 1.1f;

	// Vibrance
	float m_vibrance = 0.3f;
	const glm::vec4 m_coefficient{ 0.299f, 0.587f, 0.114f, 0.0f };

	const std::size_t MAX_NUM_LIGHTS = 128;

	const std::array<Vertex, 4> m_screenQuadVertices{
		// Positions				// GLTexture Coords
		Vertex({ -1.0f, 1.0f, 0.0f },{ 0.0f, 1.0f }),
		Vertex({ -1.0f, -1.0f, 0.0f },{ 0.0f, 0.0f }),
		Vertex({ 1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f }),
		Vertex({ 1.0f, -1.0f, 0.0f },{ 1.0f, 0.0f })
	};

	const std::array<float, 108> m_cubeVertices{
		// Back face
		-0.5f, -0.5f, -0.5f, // Bottom-left
		0.5f, 0.5f, -0.5f, // top-right
		0.5f, -0.5f, -0.5f, // bottom-right         
		0.5f, 0.5f, -0.5f, // top-right
		-0.5f, -0.5f, -0.5f, // bottom-left
		-0.5f, 0.5f, -0.5f, // top-left
		// Front face
		-0.5f, -0.5f, 0.5f, // bottom-left
		0.5f, -0.5f, 0.5f, // bottom-right
		0.5f, 0.5f, 0.5f, // top-right
		0.5f, 0.5f, 0.5f, // top-right
		-0.5f, 0.5f, 0.5f, // top-left
		-0.5f, -0.5f, 0.5f, // bottom-left
		// Left face
		-0.5f, 0.5f, 0.5f, // top-right
		-0.5f, 0.5f, -0.5f, // top-left
		-0.5f, -0.5f, -0.5f, // bottom-left
		-0.5f, -0.5f, -0.5f, // bottom-left
		-0.5f, -0.5f, 0.5f, // bottom-right
		-0.5f, 0.5f, 0.5f, // top-right
		// Right face
		0.5f, 0.5f, 0.5f, // top-left
		0.5f, -0.5f, -0.5f, // bottom-right
		0.5f, 0.5f, -0.5f, // top-right         
		0.5f, -0.5f, -0.5f, // bottom-right
		0.5f, 0.5f, 0.5f, // top-left
		0.5f, -0.5f, 0.5f, // bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f, // top-right
		0.5f, -0.5f, -0.5f, // top-left
		0.5f, -0.5f, 0.5f, // bottom-left
		0.5f, -0.5f, 0.5f, // bottom-left
		-0.5f, -0.5f, 0.5f, // bottom-right
		-0.5f, -0.5f, -0.5f, // top-right
		// Top face
		-0.5f, 0.5f, -0.5f, // top-left
		0.5f, 0.5f, 0.5f, // bottom-right
		0.5f, 0.5f, -0.5f, // top-right     
		0.5f, 0.5f, 0.5f, // bottom-right
		-0.5f, 0.5f, -0.5f, // top-left
		-0.5f, 0.5f, 0.5f // bottom-left        
	};


	const glm::vec3 LIGHT_MIN_BOUNDS = glm::vec3(-135.0f, -20.0f, -60.0f);
	const glm::vec3 LIGHT_MAX_BOUNDS = glm::vec3(135.0f, 170.0f, 60.0f);
	const float LIGHT_DELTA_TIME = -0.6f;
};
