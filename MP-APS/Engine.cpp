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

	// Models
	auto cathedral = std::make_shared<Model>("models/cathedral/sibenik.obj", "Sibenik Cathedral");
	cathedral->Scale(glm::vec3(3.0f));
	m_renderer->AddModels(cathedral);


	m_engineState = engineState::READY;
	std::cout << "\nEngine initialization complete!\n\n";
	std::cout << "**************************************************" << '\n';

	while (!m_mainWindow->ShouldClose()) {
		this->update();

		// Geometry pass
		m_renderer->DoGeometryPass();

		// Lighting pass
		m_renderer->DoDeferredLighting();

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
	m_renderer->Shutdown();
	m_mainWindow->DestroyWindow();
}
