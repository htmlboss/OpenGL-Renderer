#include "Engine.h"
#include "Model.h"

#include <GLFW/glfw3.h>

#include <iostream>

/***********************************************************************************/
Engine::Engine(const std::size_t width, const std::size_t height) : m_engineState(engineState::LOADING), 
																	m_mainWindow(new GLWindow(width, height, "MP-APS")), 
																	m_renderer(new GLRenderer(width, height)) {
}

/***********************************************************************************/
void Engine::Execute() {

	GLShaderProgram skyboxShader("Skybox Shader", { GLShader(ResourceManager::GetInstance().LoadTextFile("shaders/skyboxvs.glsl"), GLShader::ShaderType::VertexShader), 
													GLShader(ResourceManager::GetInstance().LoadTextFile("shaders/skyboxps.glsl"), GLShader::ShaderType::PixelShader) });
	skyboxShader.AddUniforms({ "projection", "view", "skybox" });

	GLShaderProgram geometryPassShader("Deferred Geometry Pass Shader", {	GLShader(ResourceManager::GetInstance().LoadTextFile("shaders/geometrypassvs.glsl"),  GLShader::ShaderType::VertexShader), 
																			GLShader(ResourceManager::GetInstance().LoadTextFile("shaders/geometrypassps.glsl"), GLShader::ShaderType::PixelShader) });
	geometryPassShader.AddUniforms({ "model", "normalMatrix", "texture_diffuse1", "texture_specular1"});

	GLShaderProgram lightingPassShader("Deferred Lighting Pass Shader", {	GLShader(ResourceManager::GetInstance().LoadTextFile("shaders/lightingpassvs.glsl"),  GLShader::ShaderType::VertexShader), 
																			GLShader(ResourceManager::GetInstance().LoadTextFile("shaders/lightingpassps.glsl"), GLShader::ShaderType::PixelShader) });
	lightingPassShader.AddUniforms({"viewPos", "gPosition", "gNormal", "gAlbedoSpec"});
	
	// Set samplers
	lightingPassShader.Bind();
	lightingPassShader.SetUniformi("gPosition", 0);
	lightingPassShader.SetUniformi("gNormal", 1);
	lightingPassShader.SetUniformi("gAlbedoSpec", 2);

	// Models
	Model cathedral("models/cathedral/sibenik.obj", "Sibenik Cathedral");


	m_engineState = engineState::READY;
	std::cout << "\nEngine initialization complete!\n\n";
	std::cout << "**************************************************" << '\n';

	while (!m_mainWindow->ShouldClose()) {
		this->update();

		// Geometry pass
		m_renderer->BeginGeometryPass();

		geometryPassShader.Bind();

		glm::mat4 model;
		model = glm::scale(model, glm::vec3(3.0f));
		geometryPassShader.SetUniform("model", model);
		geometryPassShader.SetUniform("normalMatrix", glm::transpose(glm::inverse(glm::mat3(model))));
		cathedral.Draw(geometryPassShader);

		m_renderer->EndGeometryPass();
		
		// Lighting pass
		m_renderer->BeginLightingPass();
		glClear(GL_DEPTH_BUFFER_BIT);
		lightingPassShader.Bind();
		m_renderer->m_gBuffer->BindTextures();

		lightingPassShader.SetUniform("viewPos", m_renderer->GetCameraPos());

		m_renderer->Render();
		m_renderer->RenderSkybox(skyboxShader);

		m_mainWindow->SwapBuffers();
	}

	this->shutdown();
}

/***********************************************************************************/
void Engine::update() {
	m_mainWindow->Update();
	m_timer.Update(glfwGetTime());
	m_renderer->Update(m_timer.GetDelta());
}

/***********************************************************************************/
void Engine::shutdown() const {
	m_mainWindow->DestroyWindow();
}
