#include "GLRenderer.h"
#include "GLShader.h"

#include "../Input.h"
#include "../Camera.h"
#include "../Scene.h"

#include <glm/gtc/type_ptr.hpp>
#include <random>
#include <iostream>

/***********************************************************************************/
GLRenderer::GLRenderer(const size_t width, const size_t height) : IRenderer(), m_context(), m_width(width),
																							m_height(height),
																							m_workGroupsX((width + width % 8) / 8),
																							m_workGroupsY((height + height % 8) / 8),
																							m_postProcess(width, height),
																							m_depthFBO("Depth FBO", width, height),
																							m_skybox("skybox/cloudtop/"),
																							m_skyboxShader("Skybox Shader", {	GLShader("Shaders/skyboxvs.glsl", GLShader::ShaderType::Vertex),
																																GLShader("Shaders/skyboxps.glsl", GLShader::ShaderType::Pixel) }),
																							m_terrainShader("Terrain Shader", { GLShader("Shaders/terrainvs.glsl", GLShader::ShaderType::Vertex),
																																GLShader("Shaders/terrainps.glsl", GLShader::ShaderType::Pixel) }),
																							m_depthShader("Depth Shader", {	GLShader("Shaders/depthvs.glsl", GLShader::ShaderType::Vertex),
																															GLShader("Shaders/depthps.glsl", GLShader::ShaderType::Pixel)}),
																							m_depthDebugShader("Debug Depth Shader", {	GLShader("Shaders/debugdepthvs.glsl", GLShader::ShaderType::Vertex),
																																		GLShader("Shaders/debugdepthps.glsl", GLShader::ShaderType::Pixel)}),
																							m_lightCullingShader("Light Culling Compute Shader", { GLShader("Shaders/lightcullingcs.glsl", GLShader::ShaderType::Compute)}),
																							m_lightAccumShader("Light Accumulation Shader", {	GLShader("Shaders/lightaccumulationvs.glsl", GLShader::ShaderType::Vertex),
																																				GLShader("Shaders/lightaccumulationps.glsl", GLShader::ShaderType::Pixel) })
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

	m_skyboxShader.AddUniforms({	"skybox"});
	m_terrainShader.AddUniforms({	"modelMatrix", "texture_diffuse1", "texture_diffuse2", "texture_diffuse3", "texture_diffuse4", "texture_diffuse5", 
									"viewPos", "light.direction", "light.ambient", "light.diffuse", "light.specular"});
	m_depthShader.AddUniforms({"modelMatrix"});
	m_lightAccumShader.AddUniforms({"modelMatrix", "viewPosition", "texture_diffuse1", "texture_specular1", "texture_normal1", "numberOfTilesX"});
	
	m_depthDebugShader.Bind();
	m_depthDebugShader.AddUniforms({"modelMatrix", "near", "far"});
	m_depthDebugShader.SetUniformf("near", 5.0f);
	m_depthDebugShader.SetUniformf("far", 1000.0f);

	m_lightCullingShader.Bind();
	m_lightCullingShader.AddUniforms({ "depthMap", "view", "projection", "screenSize", "lightCount" });
	m_lightCullingShader.SetUniformi("lightCount", MAX_NUM_LIGHTS);
	m_lightCullingShader.SetUniform("screenSize", glm::ivec2(m_width, m_height));

	m_lightAccumShader.Bind();
	m_lightAccumShader.SetUniformi("numberOfTilesX", m_workGroupsX);

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
	m_terrainShader.DeleteProgram();
	m_depthShader.DeleteProgram();
	m_depthDebugShader.DeleteProgram();
	m_lightCullingShader.DeleteProgram();
	m_lightAccumShader.DeleteProgram();
}

/***********************************************************************************/
void GLRenderer::Render(const Camera& camera, const RenderData& renderData) {
	const auto viewMatrix = camera.GetViewMatrix();

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
	
	// Step 1: Render the depth of the scene to a depth map
	m_depthShader.Bind();
	
	m_depthFBO.Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	renderModels(m_depthShader, renderData, true);
	m_depthFBO.Unbind();

	// Step 2: Perform light culling on point lights in the scene
	m_lightCullingShader.Bind();
	m_lightCullingShader.SetUniform("projection", m_projMatrix);
	m_lightCullingShader.SetUniform("view", viewMatrix);

	glActiveTexture(GL_TEXTURE5);
	m_lightCullingShader.SetUniformi("depthMap", 5);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);

	// Bind shader storage buffer objects for the light and indice buffers
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_visibleLightIndicesBuffer);

	// Execute compute shader
	glDispatchCompute(m_workGroupsX, m_workGroupsY, 1);

	m_postProcess.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_lightAccumShader.Bind();
	m_lightAccumShader.SetUniform("viewPosition", camera.GetPosition());
	renderModels(m_lightAccumShader, renderData, false);

	// Draw skybox
	m_skybox.Draw(m_skyboxShader);
	
	// Do post-processing
	m_postProcess.Update();

	renderQuad();

}

/***********************************************************************************/
void GLRenderer::InitView(const Camera& camera) {
	m_projMatrix = camera.GetProjMatrix(m_width, m_height);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), value_ptr(m_projMatrix));
}

/***********************************************************************************/
void GLRenderer::Update(const Camera& camera, const double delta) {

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

	UpdateLights(delta);

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

	setupLightStorageBuffer();
}

/***********************************************************************************/
void GLRenderer::setupLightStorageBuffer() {
	if (m_lightBuffer == 0) {
		std::cerr << "Error: Forward+ light buffer has not been created.\n";
		return;
	}

	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<> dist(0.0f, 1.0f);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightBuffer);
	auto* pointLights = (PointLight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
	
	for (auto i = 0; i < MAX_NUM_LIGHTS; i++) {
		auto& light = pointLights[i];
		light.Position = glm::vec4(RandomPosition(dist, gen), 1.0f);
		light.Color = glm::vec4(1.0f + dist(gen), 1.0f + dist(gen), 1.0f + dist(gen), 1.0f);
		light.RadiusAndPadding = glm::vec4(glm::vec3(0.0f), 30.0f);
	}


	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

/***********************************************************************************/
void GLRenderer::setupDepthBuffer() {
	m_depthFBO.Bind();

	if (m_depthTexture) {
		glDeleteTextures(1, &m_depthTexture);
	}
	glGenTextures(1, &m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_width, m_height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	const GLfloat borderColor[] { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	m_depthFBO.AttachTexture(m_depthTexture, GLFramebuffer::AttachmentType::DEPTH);
	m_depthFBO.DrawBuffer(GLFramebuffer::GLBuffer::NONE);
	m_depthFBO.ReadBuffer(GLFramebuffer::GLBuffer::NONE);
	
	m_depthFBO.Unbind();
}

/***********************************************************************************/
glm::vec3 GLRenderer::RandomPosition(std::uniform_real_distribution<> dis, std::mt19937_64 gen) {
	glm::vec3 position = glm::vec3(0.0);
	for (int i = 0; i < 3; i++) {
		float min = LIGHT_MIN_BOUNDS[i];
		float max = LIGHT_MAX_BOUNDS[i];
		position[i] = (GLfloat)dis(gen) * (max - min) + min;
	}

	return position;
}

/***********************************************************************************/
void GLRenderer::UpdateLights(const double dt) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightBuffer);
	PointLight *pointLights = (PointLight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	for (int i = 0; i < MAX_NUM_LIGHTS; i++) {
		PointLight &light = pointLights[i];
		float min = LIGHT_MIN_BOUNDS[1];
		float max = LIGHT_MAX_BOUNDS[1];

		light.Position.y = fmod((light.Position.y + (-4.5f * dt) - min + max), max) + min;
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}