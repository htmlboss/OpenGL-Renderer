#include "GLRenderer.h"

#include "../Input.h"
#include "../Camera.h"
#include "../Terrain.h"
#include "../Scene.h"

#include <glm/gtc/type_ptr.hpp>

/***********************************************************************************/
GLRenderer::GLRenderer(const size_t width, const size_t height) : IRenderer(), m_context(), m_depthMapFBO("Depth Map FBO", width, height),
																							m_postProcess(width, height),
																							m_skybox("skybox/cloudtop/"),
																							m_skyboxShader("Skybox Shader", {	GLShader("shaders/skyboxvs.glsl", GLShader::ShaderType::VertexShader),
																																GLShader("shaders/skyboxps.glsl", GLShader::ShaderType::PixelShader) }),
																							m_forwardShader("Forward Shader", { GLShader("shaders/forwardvs.glsl", GLShader::ShaderType::VertexShader),
																																GLShader("shaders/forwardps.glsl", GLShader::ShaderType::PixelShader) }), 
																							m_terrainShader("Terrain Shader", { GLShader("shaders/terrainvs.glsl", GLShader::ShaderType::VertexShader),
																																GLShader("shaders/terrainps.glsl", GLShader::ShaderType::PixelShader) }),
																							m_depthShader("Depth Shader", {	GLShader("shaders/depthvs.glsl", GLShader::ShaderType::VertexShader), 
																															GLShader("shaders/depthps.glsl", GLShader::ShaderType::PixelShader)})
																{
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glDepthFunc(GL_LESS);

	//m_gBuffer = std::make_unique<GBuffer>(width, height);
	m_skyboxShader.AddUniforms({	"projection", "view", "skybox"});
	m_forwardShader.AddUniforms({	"modelMatrix"});
	m_terrainShader.AddUniforms({	"modelMatrix", "lightSpaceMatrix", "texture_diffuse1", "texture_diffuse2", "texture_diffuse3", "texture_diffuse4", "texture_diffuse5", 
									"viewPos", "light.direction", "light.ambient", "light.diffuse", "light.specular"});
	m_depthShader.AddUniforms({		"modelMatrix", "lightSpaceMatrix"});

	createShadowDepthMap(1024);

	// Create uniform buffer object for projection and view matrices (same data shared to multiple shaders)
	glGenBuffers(1, &m_uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uboMatrices, 0, 2 * sizeof(glm::mat4));

	// Setup Screenquad
	glGenVertexArrays(1, &m_quadVAO);
	glGenBuffers(1, &m_quadVBO);

	glBindVertexArray(m_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_screenQuadVertices.size(), m_screenQuadVertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, TexCoords)));

	glViewport(0, 0, width, height);

	m_width = width;
	m_height = height;
}

/***********************************************************************************/
void GLRenderer::Shutdown() const {

	m_skyboxShader.DeleteProgram();
	m_forwardShader.DeleteProgram();
	m_terrainShader.DeleteProgram();
	if (m_gBuffer != nullptr) {
		m_gBuffer->Shutdown();
	}
}

/***********************************************************************************/
void GLRenderer::GetDepthBuffer() const {
	m_gBuffer->BlitDepthBuffer();
}

/***********************************************************************************/
void GLRenderer::Render(const Camera& camera, const RenderData& renderData) {
	const auto viewMatrix = camera.GetViewMatrix();

	m_postProcess.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);

	// Draw terrain
	renderData.terrain.Draw(m_terrainShader, camera.GetPosition());

	// Draw models
	m_forwardShader.Bind();
	for (const auto& model : renderData.renderList) {
		m_forwardShader.SetUniform("modelMatrix", model->GetModelMatrix());
		model->Draw(&m_forwardShader);
	}

	// Draw skybox
	m_skybox.Draw(m_skyboxShader, viewMatrix, m_projMatrix);
	
	// Do post-processing
	m_postProcess.Update();
	renderQuad();

}

/***********************************************************************************/
void GLRenderer::renderQuad() const {
	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/***********************************************************************************/
void GLRenderer::createShadowDepthMap(const size_t shadowResolution) {
#ifdef _DEBUG
	assert(shadowResolution % 2 == 0);
#endif
	if (m_depthMap) {
		glDeleteTextures(1, &m_depthMap);
	}
	glGenTextures(1, &m_depthMap);
	glBindTexture(GL_TEXTURE_2D, m_depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowResolution, shadowResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	m_depthMapFBO.Bind();
	m_depthMapFBO.AttachTexture(m_depthMap, GLFramebuffer::AttachmentType::DEPTH);
	m_depthMapFBO.DrawBuffer(GLFramebuffer::GLBuffer::NONE);
	m_depthMapFBO.ReadBuffer(GLFramebuffer::GLBuffer::NONE);
}

/***********************************************************************************/
void GLRenderer::InitView(const Camera& camera) {
	m_projMatrix = camera.GetProjMatrix(m_width, m_height);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), value_ptr(m_projMatrix));
}

/***********************************************************************************/
void GLRenderer::Update(const double deltaTime, const Camera& camera) {

	// Window size changed.
	if (Input::GetInstance().ShouldResize()) {
		m_width = Input::GetInstance().GetWidth();
		m_height = Input::GetInstance().GetHeight();

		m_projMatrix = camera.GetProjMatrix(m_width, m_height);
		InitView(camera);
		glViewport(0, 0, m_width, m_height);
		//m_gBuffer->Resize(m_width, m_height);
		m_postProcess.Resize(m_width, m_height);

		m_depthMapFBO.Reset(m_width, m_height);
		createShadowDepthMap(1024);
	}

	// Update view matrix inside UBO
	const auto view = camera.GetViewMatrix();
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/***********************************************************************************/
void GLRenderer::DoGeometryPass() {

	m_gBuffer->BindGeometryShader();
	m_gBuffer->BindGBuffer();

	auto shader = m_gBuffer->GetGeometryShader();

	m_gBuffer->UnBindGBuffer();
}

/***********************************************************************************/
void GLRenderer::DoDeferredLighting() const {

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	m_gBuffer->BindLightingShader();
	m_gBuffer->BindPosNormDiffSpec(); // Textures 0, 1, 2
	//m_gBuffer->SetCameraPos(m_camera->GetPosition());
}
