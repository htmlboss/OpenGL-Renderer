#include "RenderSystem.h"

#include "../Graphics/GLShader.h"
#include "../Camera.h"

#include "../Input.h"
#include "../SceneBase.h"

#include <pugixml.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GLFW/glfw3.h>

#include <iostream>

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
	m_shadowMapResolution = rendererNode.attribute("shadowResolution").as_uint();

	m_hdrFBO.Init("HDR FBO");
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
	
	setupScreenquad();
	setupTextureSamplers();
	setupShadowMap();
	setupPostProcessing();
	

#ifdef _DEBUG
	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
#endif
	setDefaultState();
	glEnable(GL_MULTISAMPLE);
	
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
	pbrShader.SetUniformi("roughnessMap", 6).SetUniformi("shadowMap", 7).SetUniformf("bloomThreshold", 1.0f);//.SetUniformi("aoMap", 7);
	
	auto& skyboxShader = m_shaderCache.at("SkyboxShader");
	skyboxShader.Bind();
	skyboxShader.SetUniformi("environmentMap", 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glViewport(0, 0, width, height);

}

/***********************************************************************************/
void RenderSystem::Update(const Camera& camera) {

	// Window size changed.
	if (Input::GetInstance().ShouldResize()) {
		m_width = Input::GetInstance().GetWidth();
		m_height = Input::GetInstance().GetHeight();

		UpdateView(camera);
		glViewport(0, 0, m_width, m_height);
	}

	// Update view matrix inside UBO
	const auto view = camera.GetViewMatrix();
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), glm::value_ptr(view));
}

/***********************************************************************************/
void RenderSystem::Shutdown() const {
	for (const auto& shader : m_shaderCache) {
		shader.second.DeleteProgram();
	}
}

/***********************************************************************************/
void RenderSystem::Render(const Camera& camera, RenderListIterator renderListBegin, RenderListIterator renderListEnd, const SceneBase& scene, const bool globalWireframe) {
	
	setDefaultState();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);

	// Get the shaders we need (static vars initialized during first render call).
	static auto& pbrShader = m_shaderCache.at("PBRShader");
	static auto& blurShader = m_shaderCache.at("GaussianBlurShader");
	static auto& bloomBlendShader = m_shaderCache.at("BloomBlendShader");
	static auto& skyboxShader = m_shaderCache.at("SkyboxShader");
	
	// Shadow mapping
	renderShadowMap(scene, renderListBegin, renderListEnd);

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
	glActiveTexture(GL_TEXTURE7);
	glBindTexture(GL_TEXTURE_2D, m_shadowDepthTexture);

	pbrShader.Bind();
	pbrShader.SetUniform("camPos", camera.GetPosition()).SetUniformi("wireframe", globalWireframe).SetUniform("lightSpaceMatrix", m_lightSpaceMatrix);
	pbrShader.SetUniform("directionalLight", scene.m_staticDirectionalLights[0].Direction).SetUniform("lightColor", scene.m_staticDirectionalLights[0].Color);

	renderModelsWithTextures(pbrShader, renderListBegin, renderListEnd);

	// Draw skybox
	skyboxShader.Bind();
	glActiveTexture(GL_TEXTURE0);
	m_skybox.Draw();

	// Do bloom
	blurShader.Bind();
	bool horizontal = true, first_iteration = true;
	static constexpr unsigned int amount = 10;
	for (unsigned int i = 0; i < amount; ++i) {
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
void RenderSystem::UpdateView(const Camera& camera) {
	m_projMatrix = camera.GetProjMatrix(m_width, m_height);
	glBindBuffer(GL_UNIFORM_BUFFER, m_uboMatrices);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), glm::value_ptr(m_projMatrix));
}

/***********************************************************************************/
void RenderSystem::setDefaultState() {
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

/***********************************************************************************/
void RenderSystem::renderModelsWithTextures(GLShaderProgram& shader, RenderListIterator renderListBegin, RenderListIterator renderListEnd) const {
	glBindSampler(m_samplerPBRTextures, 3);
	glBindSampler(m_samplerPBRTextures, 4);
	glBindSampler(m_samplerPBRTextures, 5);
	glBindSampler(m_samplerPBRTextures, 6);
	// glBindSampler(m_samplerPBRTextures, 7);

	auto begin{ renderListBegin };

	while (begin != renderListEnd) {
		shader.SetUniform("modelMatrix", (*begin)->GetModelMatrix());
		
		const auto& meshes{ (*begin)->GetMeshes() };
		for (const auto& mesh : meshes) {
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(PBRMaterial::ALBEDO));
			glActiveTexture(GL_TEXTURE4);
			glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(PBRMaterial::NORMAL));
			glActiveTexture(GL_TEXTURE5);
			glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(PBRMaterial::METALLIC));
			glActiveTexture(GL_TEXTURE6);
			glBindTexture(GL_TEXTURE_2D, mesh.Material->GetParameterTexture(PBRMaterial::ROUGHNESS));
			
			//glActiveTexture(GL_TEXTURE7);
			//glBindTexture(GL_TEXTURE_2D, mesh.Material.AOMap);

			mesh.VAO.Bind();
			glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		++begin;
	}

	glBindSampler(m_samplerPBRTextures, 0);
}

/***********************************************************************************/
void RenderSystem::renderModelsNoTextures(GLShaderProgram& shader, RenderListIterator renderListBegin, RenderListIterator renderListEnd) const {
	auto begin{ renderListBegin };

	while(begin != renderListEnd) {
		shader.SetUniform("modelMatrix", (*begin)->GetModelMatrix());

		const auto& meshes{ (*begin)->GetMeshes() };
		for (const auto& mesh : meshes) {
			mesh.VAO.Bind();
			glDrawElements(GL_TRIANGLES, mesh.IndexCount, GL_UNSIGNED_INT, nullptr);
			glBindTexture(GL_TEXTURE_2D, 0);
		}

		++begin;
	}
}

/***********************************************************************************/
void RenderSystem::renderQuad() const {
	m_quadVAO.Bind();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}

/***********************************************************************************/
void RenderSystem::renderShadowMap(const SceneBase& scene, RenderListIterator renderListBegin, RenderListIterator renderListEnd) {
	static auto& shadowDepthShader = m_shaderCache.at("ShadowDepthShader");
	shadowDepthShader.Bind();
	static constexpr float near_plane = 0.0f, far_plane = 100.0f;
	static const glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, 50.0f, -50.0f, near_plane, far_plane);

	static const auto& lightView = glm::lookAt(scene.m_staticDirectionalLights[0].Direction, 
								  glm::vec3(0.0f), 
								  glm::vec3(0.0f, -1.0f, 0.0f));

	m_lightSpaceMatrix = lightProjection * lightView;

	shadowDepthShader.SetUniform("lightSpaceMatrix", m_lightSpaceMatrix);
	
	glCullFace(GL_FRONT); // Solve peter-panning
	glViewport(0, 0, m_shadowMapResolution, m_shadowMapResolution);
	m_shadowDepthFBO.Bind();
	glClear(GL_DEPTH_BUFFER_BIT);

	renderModelsNoTextures(shadowDepthShader, renderListBegin, renderListEnd);

	m_shadowDepthFBO.Unbind();
	glViewport(0, 0, m_width, m_height);
	glCullFace(GL_BACK);
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
void RenderSystem::setupShadowMap() {
	m_shadowDepthFBO.Init("Shadow Depth FBO");
	m_shadowDepthFBO.Bind();

	if (m_shadowDepthTexture) {
		glDeleteTextures(1, &m_shadowDepthTexture);
	}
	glGenTextures(1, &m_shadowDepthTexture);
	glBindTexture(GL_TEXTURE_2D, m_shadowDepthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_shadowMapResolution, m_shadowMapResolution, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER); // Clamp to border to fix over-sampling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	const float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor); 

	m_shadowDepthFBO.AttachTexture(m_shadowDepthTexture, GLFramebuffer::AttachmentType::DEPTH);
	m_shadowDepthFBO.DrawBuffer(GLFramebuffer::GLBuffer::NONE);
	m_shadowDepthFBO.ReadBuffer(GLFramebuffer::GLBuffer::NONE);
	
	m_shadowDepthFBO.Unbind();
}

/***********************************************************************************/
void RenderSystem::setupPostProcessing() {
	
	// HDR
	m_hdrFBO.Reset();
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
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
	
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
		m_pingPongFBOs[i].Reset();
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
