#include "GLRenderer.h"

#include "../Input.h"
#include "../Camera.h"
#include "../Terrain.h"

#include <glm/gtc/type_ptr.hpp>

/***********************************************************************************/
GLRenderer::GLRenderer(const size_t width, const size_t height) : IRenderer(), m_context(), m_skybox("skybox/cloudtop/"),
																							m_skyboxShader("Skybox Shader", {	GLShader("shaders/skyboxvs.glsl", GLShader::ShaderType::VertexShader),
																																GLShader("shaders/skyboxps.glsl", GLShader::ShaderType::PixelShader) }),
																							m_forwardShader("Forward Shader", { GLShader("shaders/forwardvs.glsl", GLShader::ShaderType::VertexShader),
																																GLShader("shaders/forwardps.glsl", GLShader::ShaderType::PixelShader) }), 
																							m_terrainShader("Terrain Shader", { GLShader("shaders/terrainvs.glsl", GLShader::ShaderType::VertexShader),
																																GLShader("shaders/terrainps.glsl", GLShader::ShaderType::PixelShader) }) 
																{
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glDepthFunc(GL_LESS);

	//m_gBuffer = std::make_unique<GBuffer>(width, height);
	m_postProcess = std::make_unique<GLPostProcess>(width, height);
	
	m_skyboxShader.AddUniforms({"projection", "view", "skybox"});
	m_forwardShader.AddUniforms({"modelMatrix"});
	m_terrainShader.AddUniforms({"modelMatrix", "texture_diffuse1", "texture_diffuse2", "texture_diffuse3", "texture_diffuse4", "texture_diffuse5", "viewPos", "light.direction", "light.ambient", "light.diffuse", "light.specular"});

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
void GLRenderer::GetDepthBuffer() const { m_gBuffer->BlitDepthBuffer(); }

/***********************************************************************************/
void GLRenderer::Render(const Camera& camera, Terrain& terrain, const std::vector<ModelPtr>& renderList) {
	const auto viewMatrix = camera.GetViewMatrix();

	m_postProcess->Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);

	// Draw terrain
	terrain.Draw(m_terrainShader, camera.GetPosition());

	m_forwardShader.Bind();
	for (const auto& model : renderList) {
		m_forwardShader.SetUniform("modelMatrix", model->GetModelMatrix());
		model->Draw(&m_forwardShader);
	}

	m_skybox.Draw(m_skyboxShader, viewMatrix, m_projMatrix);
	m_postProcess->Update();
	renderQuad();

}

/***********************************************************************************/
void GLRenderer::renderQuad() const {
	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
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
		m_postProcess->Resize(m_width, m_height);
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
