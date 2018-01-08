#include "RenderSystem.h"

#include "../Graphics/GLShader.h"

#include "../Input.h"
#include "../SceneBase.h"

#include <pugixml.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

#include <iostream>

/***********************************************************************************/
RenderSystem::RenderSystem() :	m_width{0},
							m_height{0},
							m_workGroupsX{0},
							m_workGroupsY{0} {}

/***********************************************************************************/
void RenderSystem::Init(const pugi::xml_node& rendererNode) {
	
	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
		std::cerr << "Failed to start GLAD.";
		std::abort();
	}

#ifdef _DEBUG
	std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << '\n';
	std::cout << "GLSL Version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << '\n';
	std::cout << "OpenGL Driver Vendor: " << glGetString(GL_VENDOR) << '\n';
	std::cout << "OpenGL Renderer: " << glGetString(GL_RENDERER) << '\n';
#endif

	const auto width = rendererNode.attribute("width").as_uint();
	const auto height = rendererNode.attribute("height").as_uint();

	m_width = width;
	m_height = height;

	m_workGroupsX = (width + width % 8) / 8;
	m_workGroupsY = (height + height % 8) / 8;

	m_depthFBO.Init("Depth FBO", width, height);
	m_hdrFBO.Init("HDR FBO", width, height);
	m_skybox.Init("Data/hdri/barcelona.hdr", 2048);

	// Compile all shader programs in config.xml
	for (auto program = rendererNode.child("Program"); program; program = program.next_sibling("Program")) {

		// Get all shader files that make up the program
		std::vector<GLShader> shaders;
		for (auto shader = program.child("Shader"); shader; shader = shader.next_sibling("Shader")) {
			shaders.emplace_back(shader.attribute("path").as_string(), shader.attribute("type").as_string());
		}

		// Compile and cache shader program
		m_shaderCache.try_emplace(program.attribute("name").as_string(), program.attribute("name").as_string(), shaders);
	}

	setupLightBuffers();
	setupScreenquad();
	setupTextureSamplers();
	setupDepthBuffer();
	setupPostProcessing();

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glEnable(GL_MULTISAMPLE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	// Create uniform buffer object for projection and view matrices
	// so same data shared to multiple shader programs.
	glGenBuffers(1, &m_uboMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
	glBindBufferRange(GL_UNIFORM_BUFFER, 0, m_uboMatrices, 0, 2 * sizeof(glm::mat4));

	auto& pbrShader = m_shaderCache.at("PBRShader");
	pbrShader.Bind();
	pbrShader.SetUniformi("irradianceMap", 0).SetUniformi("prefilterMap", 1).SetUniformi("brdfLUT", 2);
	pbrShader.SetUniformi("albedoMap", 3).SetUniformi("normalMap", 4).SetUniformi("metallicMap", 5);
	pbrShader.SetUniformi("roughnessMap", 6).SetUniformf("bloomThreshold", 1.0f);//.SetUniformi("aoMap", 7);

	auto& skyboxShader = m_shaderCache.at("SkyboxShader");
	skyboxShader.Bind();
	skyboxShader.SetUniformi("environmentMap", 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, width, height);
}

/***********************************************************************************/
void RenderSystem::Shutdown() const {
	for (const auto& shader : m_shaderCache) {
		shader.second.DeleteProgram();
	}
}

/***********************************************************************************/
void RenderSystem::Render(const SceneBase& scene, const bool wireframe) {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);

	const auto viewMatrix = scene.m_camera.GetViewMatrix();

	// Get the shaders we need (static vars initialized during first render call).
	static auto& depthShader = m_shaderCache.at("DepthPassShader");
	static auto& lightCullShader = m_shaderCache.at("LightCullShader");
	static auto& pbrShader = m_shaderCache.at("PBRShader");
	static auto& blurShader = m_shaderCache.at("GaussianBlurShader");
	static auto& bloomBlendShader = m_shaderCache.at("BloomBlendShader");
	static auto& skyboxShader = m_shaderCache.at("SkyboxShader");
	
	// Step 1: Render the depth of the scene to a depth map
	depthShader.Bind();
	
	m_depthFBO.Bind();
	glClear(GL_DEPTH_BUFFER_BIT);
	renderModelsNoTextures(depthShader, scene.m_renderList);
	m_depthFBO.Unbind();

	// Step 2: Perform light culling on point lights in the scene
	lightCullShader.Bind();
	lightCullShader.SetUniform("projection", m_projMatrix);
	lightCullShader.SetUniform("view", viewMatrix);

	glActiveTexture(GL_TEXTURE8);
	lightCullShader.SetUniformi("depthMap", 9);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);

	// Bind shader storage buffer objects for the light and index buffers
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_lightBuffer);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_visibleLightIndicesBuffer);

	// Execute compute shader
	glDispatchCompute(m_workGroupsX, m_workGroupsY, 1);

	// Regular rendering
	m_hdrFBO.Bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Bind pre-computed IBL data
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox.GetIrradianceMap());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_skybox.GetPrefilterMap());
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_skybox.GetBRDFLUT());

	pbrShader.Bind();
	pbrShader.SetUniform("camPos", scene.GetCamera().GetPosition()).SetUniformi("wireframe", wireframe);

	renderModelsWithTextures(pbrShader, scene.m_renderList);

	// Draw skybox
	skyboxShader.Bind();
	glActiveTexture(GL_TEXTURE0);
	m_skybox.Draw();

	// Do bloom
	blurShader.Bind();
	bool horizontal = true, first_iteration = true;
	static const unsigned int amount = 10;
	for (unsigned int i = 0; i < amount; i++) {
		m_pingPongFBOs[horizontal].Bind();

		blurShader.SetUniformi("horizontal", horizontal);

		// Bind texture of other framebuffer (or scene if first iteration)
		glBindTexture(GL_TEXTURE_2D, first_iteration ? m_brightnessThresholdColorBuffer : m_pingPongColorBuffers[!horizontal]);

		renderQuad();
		horizontal = !horizontal;

		if (first_iteration) {
			first_iteration = false;
		}
	}

	// Blend bloom with original image and apply other post-processing effects
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	bloomBlendShader.Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_hdrColorBuffer);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_pingPongColorBuffers[!horizontal]);
	renderQuad();
	
}

/***********************************************************************************/
// TODO: This needs to be gutted and put elsewhere
void RenderSystem::InitView(const Camera& camera) {
	m_projMatrix = camera.GetProjMatrix(m_width, m_height);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), value_ptr(m_projMatrix));
}

/***********************************************************************************/
void RenderSystem::Update(const Camera& camera, const double delta) {

	// Window size changed.
	if (Input::GetInstance().ShouldResize()) {
		m_width = Input::GetInstance().GetWidth();
		m_height = Input::GetInstance().GetHeight();

		m_projMatrix = camera.GetProjMatrix(m_width, m_height);
		InitView(camera);
		glViewport(0, 0, m_width, m_height);
		m_depthFBO.Resize(m_width, m_height);
	}

	// Update view matrix inside UBO
	const auto view = camera.GetViewMatrix();
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));

	//UpdateLights(delta);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

/***********************************************************************************/
void RenderSystem::renderModelsWithTextures(GLShaderProgram& shader, const std::vector<ModelPtr>& renderList) const {
	glBindSampler(m_samplerPBRTextures, 3);
	glBindSampler(m_samplerPBRTextures, 4);
	glBindSampler(m_samplerPBRTextures, 5);
	glBindSampler(m_samplerPBRTextures, 6);
	// glBindSampler(m_samplerPBRTextures, 7);

	for (const auto& model : renderList) {
		shader.SetUniform("modelMatrix", model->GetModelMatrix());
		
		const auto meshes = model->GetMeshes();
		for (const auto& mesh : meshes) {
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, mesh.Material.AlbedoMap);
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, mesh.Material.NormalMap);
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, mesh.Material.MetallicMap);
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, mesh.Material.RoughnessMap);
			//glActiveTexture(GL_TEXTURE7);
			//glBindTexture(GL_TEXTURE_2D, mesh.Material.AOMap);

			mesh.VAO.Bind();
			glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}

	glBindSampler(m_samplerPBRTextures, 0);
}

/***********************************************************************************/
void RenderSystem::renderModelsNoTextures(GLShaderProgram & shader, const std::vector<ModelPtr>& renderList) const {
	
	for (const auto& model : renderList) {
		shader.SetUniform("modelMatrix", model->GetModelMatrix());

		const auto meshes = model->GetMeshes();
		for (const auto& mesh : meshes) {
			mesh.VAO.Bind();
			glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
	}
}

/***********************************************************************************/
void RenderSystem::renderQuad() const {
	m_quadVAO.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/***********************************************************************************/
void RenderSystem::setupScreenquad() {
	const std::array<Vertex, 4> screenQuadVertices {
		// Positions				// GLTexture Coords
		Vertex({ -1.0f, 1.0f, 0.0f },{ 0.0f, 1.0f }),
		Vertex({ -1.0f, -1.0f, 0.0f },{ 0.0f, 0.0f }),
		Vertex({ 1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f }),
		Vertex({ 1.0f, -1.0f, 0.0f },{ 1.0f, 0.0f })
	};

	m_quadVAO.Init();
	m_quadVAO.Bind();
	m_quadVAO.AttachBuffer(GLVertexArray::BufferType::ARRAY, 
		sizeof(Vertex) * screenQuadVertices.size(), 
		GLVertexArray::DrawMode::STATIC, 
		screenQuadVertices.data());
	m_quadVAO.EnableAttribute(0, 3, sizeof(Vertex), nullptr);
	m_quadVAO.EnableAttribute(1, 2, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, TexCoords)));
}

/***********************************************************************************/
void RenderSystem::setupLightBuffers() {
	const auto numberOfTiles = m_workGroupsX * m_workGroupsY;

	glGenBuffers(1, &m_lightBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, MAX_NUM_LIGHTS * sizeof(StaticPointLight), nullptr, GL_DYNAMIC_DRAW);

	glGenBuffers(1, &m_visibleLightIndicesBuffer);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_visibleLightIndicesBuffer);
	glBufferData(GL_SHADER_STORAGE_BUFFER, numberOfTiles * sizeof(VisibleLightIndex) * MAX_NUM_LIGHTS, nullptr, GL_STATIC_DRAW);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	setupLightStorageBuffer();
}

/***********************************************************************************/
void RenderSystem::setupTextureSamplers() {
	// Find max supported hardware anisotropy
	float aniso = 0.0f;
	glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &aniso);

	// Sampler for PBR textures used on meshes
	glGenSamplers(1, &m_samplerPBRTextures);
	glSamplerParameteri(m_samplerPBRTextures, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glSamplerParameteri(m_samplerPBRTextures, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glSamplerParameteri(m_samplerPBRTextures, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glSamplerParameteri(m_samplerPBRTextures, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glSamplerParameterf(m_samplerPBRTextures, GL_TEXTURE_MAX_ANISOTROPY_EXT, aniso);

}

/***********************************************************************************/
void RenderSystem::setupLightStorageBuffer() {
	if (m_lightBuffer == 0) {
		std::cerr << "Error: Forward+ light buffer has not been created.\n";
		std::abort();
	}

	std::random_device rd;
	std::mt19937_64 gen(rd());
	std::uniform_real_distribution<> dist(0.0f, 1.0f);

	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightBuffer);
	auto* pointLights = (StaticPointLight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);
	
	/*
	for (auto i = 0; i < MAX_NUM_LIGHTS; i++) {
		auto& light = pointLights[i];
		light.Position = glm::vec4(RandomPosition(dist, gen), 1.0f);
		light.Color = glm::vec4(1.0f + dist(gen), 1.0f + dist(gen), 1.0f + dist(gen), 1.0f);
		light.RadiusAndPadding = glm::vec4(glm::vec3(0.0f), 30.0f);
	}
	*/
	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

/***********************************************************************************/
void RenderSystem::setupDepthBuffer() {
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
void RenderSystem::setupPostProcessing() {
	
	// HDR
	m_hdrFBO.Reset(m_width, m_height);
	m_hdrFBO.Bind();

	// Regular old HDR
	glGenTextures(1, &m_hdrColorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_hdrColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	// For extracting bright parts of image for bloom
	glGenTextures(1, &m_brightnessThresholdColorBuffer);
	glBindTexture(GL_TEXTURE_2D, m_brightnessThresholdColorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	GLuint rboDepth;
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, m_width, m_height);
	
	// Attach buffers
	m_hdrFBO.AttachTexture(m_hdrColorBuffer, GLFramebuffer::AttachmentType::COLOR0);
	m_hdrFBO.AttachTexture(m_brightnessThresholdColorBuffer, GLFramebuffer::AttachmentType::COLOR1);
	m_hdrFBO.AttachRenderBuffer(rboDepth, GLFramebuffer::AttachmentType::DEPTH);

	// Enable MRT
	const unsigned int attachments[2] { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	m_hdrFBO.DrawBuffers(attachments);

	// Bloom
	glGenTextures(2, m_pingPongColorBuffers.data());
	for (auto i = 0; i < 2; i++) {
		m_pingPongFBOs[i].Reset(m_width, m_height);
		m_pingPongFBOs[i].Bind();

		glBindTexture(GL_TEXTURE_2D, m_pingPongColorBuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, m_width, m_height, 0, GL_RGB, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		m_pingPongFBOs[i].AttachTexture(m_pingPongColorBuffers[i], GLFramebuffer::AttachmentType::COLOR0);
	}

	// Configure bloom + blur shader
	auto& blurShader = m_shaderCache.at("GaussianBlurShader");
	blurShader.Bind();
	blurShader.SetUniformi("image", 0);

	auto& bloomBlendShader = m_shaderCache.at("BloomBlendShader");
	bloomBlendShader.Bind();
	
	bloomBlendShader.SetUniformi("scene", 0).SetUniformi("bloomBlur", 1);

	bloomBlendShader.SetUniformf("vibranceAmount", m_vibrance);
	bloomBlendShader.SetUniform("vibranceCoefficient", m_coefficient);


}

/***********************************************************************************/
glm::vec3 RenderSystem::RandomPosition(std::uniform_real_distribution<> dis, std::mt19937_64 gen) {
	auto position = glm::vec3(0.0);
	for (auto i = 0; i < 3; i++) {
		const auto min = LIGHT_MIN_BOUNDS[i];
		const auto max = LIGHT_MAX_BOUNDS[i];
		position[i] = (GLfloat)dis(gen) * (max - min) + min;
	}

	return position;
}

/***********************************************************************************/
void RenderSystem::UpdateLights(const double dt) {
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_lightBuffer);
	StaticPointLight *pointLights = (StaticPointLight*)glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_READ_WRITE);

	for (int i = 0; i < MAX_NUM_LIGHTS; i++) {
		StaticPointLight &light = pointLights[i];
		float min = LIGHT_MIN_BOUNDS[1];
		float max = LIGHT_MAX_BOUNDS[1];

		light.Position.y = fmod((light.Position.y + (-4.5f * dt) - min + max), max) + min;
	}

	glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}