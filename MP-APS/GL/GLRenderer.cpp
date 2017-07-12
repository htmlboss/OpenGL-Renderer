#include "GLRenderer.h"
#include "GLShader.h"

#include "../Input.h"
#include "../Camera.h"
#include "../Scene.h"

#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <random>

/***********************************************************************************/
GLRenderer::GLRenderer(const size_t width, const size_t height) : IRenderer(), m_context(), m_width(width),
																							m_height(height),
																							m_workGroupsX((width + width % 16) / 16),
																							m_workGroupsY((height + height % 16) / 16),
																							m_postProcess(width, height),
																							m_depthFBO("Depth FBO", width, height),
																							m_skybox("skybox/cloudtop/"),
																							m_skyboxShader("Skybox Shader", {	GLShader("shaders/skyboxvs.glsl", GLShader::ShaderType::VertexShader),
																																GLShader("shaders/skyboxps.glsl", GLShader::ShaderType::PixelShader) }),
																							m_forwardShader("Forward Shader", { GLShader("shaders/forwardvs.glsl", GLShader::ShaderType::VertexShader),
																																GLShader("shaders/forwardps.glsl", GLShader::ShaderType::PixelShader) }), 
																							m_terrainShader("Terrain Shader", { GLShader("shaders/terrainvs.glsl", GLShader::ShaderType::VertexShader),
																																GLShader("shaders/terrainps.glsl", GLShader::ShaderType::PixelShader) }),
																							m_depthShader("Depth Shader", {	GLShader("shaders/depthvs.glsl", GLShader::ShaderType::VertexShader),
																															GLShader("shaders/depthps.glsl", GLShader::ShaderType::PixelShader)}),
																							m_depthDebugShader("Debug Depth Shader", {	GLShader("shaders/debugdepthvs.glsl", GLShader::ShaderType::VertexShader),
																																		GLShader("shaders/debugdepthps.glsl", GLShader::ShaderType::PixelShader)})
																							{
	std::cout << "OpenGL Version: " << m_context.GetGLVersion() << '\n';
	std::cout << "GLSL Version: " << m_context.GetGLSLVersion() << '\n';
	std::cout << "OpenGL Vendor: " << m_context.GetGLVendor() << '\n';
	std::cout << "OpenGL Renderer: " << m_context.GetGLRenderer() << std::endl << std::endl;

	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glDepthFunc(GL_LESS);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	m_skyboxShader.AddUniforms({	"projection", "view", "skybox"});
	m_forwardShader.AddUniforms({	"modelMatrix", "texture_diffuse1", "texture_specular1", "viewPos", "light.direction", "light.ambient", "light.diffuse", "light.specular"});
	m_terrainShader.AddUniforms({	"modelMatrix", "texture_diffuse1", "texture_diffuse2", "texture_diffuse3", "texture_diffuse4", "texture_diffuse5", 
									"viewPos", "light.direction", "light.ambient", "light.diffuse", "light.specular"});
	m_depthShader.AddUniforms({"modelMatrix"});
	m_depthDebugShader.AddUniforms({"modelMatrix", "near", "far"});
	
	m_depthDebugShader.Bind();
	m_depthDebugShader.SetUniformf("near", 5.0f);
	m_depthDebugShader.SetUniformf("far", 1000.0f);

	setupLightBuffers();
	setupScreenquad();
	setupDepthBuffer();

	// Create uniform buffer object for projection and view matrices (same data shared to multiple shaders)
	glGenBuffers(1, &m_uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uboMatrices, 0, 2 * sizeof(glm::mat4));

	glViewport(0, 0, width, height);
}

/***********************************************************************************/
void GLRenderer::Shutdown() const {

	m_skyboxShader.DeleteProgram();
	m_forwardShader.DeleteProgram();
	m_terrainShader.DeleteProgram();
	m_depthShader.DeleteProgram();
	m_depthDebugShader.DeleteProgram();
}

/***********************************************************************************/
void GLRenderer::Render(const Camera& camera, const RenderData& renderData) {
	const auto viewMatrix = camera.GetViewMatrix();

	//m_postProcess.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);

	/*
	renderTerrain(renderData, camera.GetPosition(), false);

	m_forwardShader.Bind();
	m_forwardShader.SetUniform("viewPos", camera.GetPosition());
	m_forwardShader.SetUniform("light.direction", renderData.Sun.Direction);
	m_forwardShader.SetUniform("light.ambient", { 0.1f, 0.1f, 0.1f });
	m_forwardShader.SetUniform("light.diffuse", renderData.Sun.Color);
	m_forwardShader.SetUniform("light.specular", glm::vec3(1.0f));
	*/
	renderModels(m_depthDebugShader, renderData, true);

	// Draw skybox
	//m_skybox.Draw(m_skyboxShader, viewMatrix, m_projMatrix);
	
	// Do post-processing
	//m_postProcess.Update();

	renderQuad();
}

/***********************************************************************************/
void GLRenderer::InitView(const Camera& camera) {
	m_projMatrix = camera.GetProjMatrix(m_width, m_height);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), value_ptr(m_projMatrix));
}

/***********************************************************************************/
void GLRenderer::Update(const Camera& camera) {

	// Window size changed.
	if (Input::GetInstance().ShouldResize()) {
		m_width = Input::GetInstance().GetWidth();
		m_height = Input::GetInstance().GetHeight();

		m_projMatrix = camera.GetProjMatrix(m_width, m_height);
		InitView(camera);
		glViewport(0, 0, m_width, m_height);
		m_postProcess.Resize(m_width, m_height);
		m_depthFBO.Resize(m_width, m_height);
	}

	// Update view matrix inside UBO
	const auto view = camera.GetViewMatrix();
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/***********************************************************************************/
void GLRenderer::renderTerrain(const RenderData& renderData, const glm::vec3& cameraPos, const bool depthPass) {

	m_terrainShader.Bind();

	if (!depthPass) {
		m_terrainShader.SetUniform("modelMatrix", renderData.terrain.GetModelMatrix());
		m_terrainShader.SetUniform("light.direction", renderData.Sun.Direction);
		m_terrainShader.SetUniform("light.ambient", { 0.1f, 0.1f, 0.1f });
		m_terrainShader.SetUniform("light.diffuse", renderData.Sun.Color);
		m_terrainShader.SetUniform("light.specular", glm::vec3(1.0f));
		m_terrainShader.SetUniform("viewPos", cameraPos);

		renderData.terrain.GetMeshes()[0].BindTextures(&m_terrainShader);
	}

	const auto data = renderData.terrain.GetMeshes()[0].GetRenderData();
	data.VAO.Bind();
	glDrawElements(GL_TRIANGLES, data.Indices.size(), GL_UNSIGNED_INT, nullptr);
}

/***********************************************************************************/
void GLRenderer::renderModels(GLShaderProgram& shader, const RenderData& renderData, const bool depthPass) const {

	for (const auto& model : renderData.renderList) {
		shader.SetUniform("modelMatrix", model->GetModelMatrix());

		auto meshes = model->GetMeshes();
		for (auto& mesh : meshes) {
			const auto data = mesh.GetRenderData();

			if (!depthPass) {
				mesh.BindTextures(&shader);
			}
			data.VAO.Bind();
			glDrawElements(GL_TRIANGLES, data.Indices.size(), GL_UNSIGNED_INT, nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

/***********************************************************************************/
void GLRenderer::renderQuad() const {
	glBindVertexArray(m_quadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/***********************************************************************************/
void GLRenderer::setupScreenquad() {
	glGenVertexArrays(1, &m_quadVAO);
	glGenBuffers(1, &m_quadVBO);

	glBindVertexArray(m_quadVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m_screenQuadVertices.size(), m_screenQuadVertices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<GLvoid*>(offsetof(Vertex, TexCoords)));
}

/***********************************************************************************/
void GLRenderer::setupLightBuffers() {
	const auto numberOfTiles = m_workGroupsX * m_workGroupsY;

	glGenBuffers(1, &m_lightBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_NUM_LIGHTS * sizeof(PointLight), nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_visibleLightIndicesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_visibleLightIndicesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfTiles * sizeof(VisibleLightIndex) * MAX_NUM_LIGHTS, nullptr, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

/***********************************************************************************/
void GLRenderer::setupLightStorageBuffer() {
	if (m_lightBuffer == 0) {
		std::cerr << "Error: Forward+ light buffer has not been created.\n";
		return;
	}

	std::random_device rd;
	const std::mt19937_64 gen(rd());
	std::uniform_real_distribution<> dist(0.0f, 1.0f);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightBuffer);


	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

/***********************************************************************************/
void GLRenderer::setupDepthBuffer() const {
	m_depthFBO.Bind();

	GLuint depthTex;
	glGenTextures(1, &depthTex);
	glBindTexture(GL_TEXTURE_2D, depthTex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	const GLfloat borderColor[] { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	m_depthFBO.AttachTexture(depthTex, GLFramebuffer::AttachmentType::DEPTH);
	m_depthFBO.DrawBuffer(GLFramebuffer::GLBuffer::NONE);
	m_depthFBO.ReadBuffer(GLFramebuffer::GLBuffer::NONE);
	
	m_depthFBO.Unbind();
}
