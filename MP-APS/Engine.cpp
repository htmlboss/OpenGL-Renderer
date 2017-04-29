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

	// Models
	auto dragon = std::make_shared<Model>("models/dragon/dragon.obj", "Dragon");
	dragon->Scale(glm::vec3(1.0f));
	m_renderer->AddModels(dragon);


	m_engineState = engineState::READY;
	std::cout << "\nEngine initialization complete!\n\n";
	std::cout << "**************************************************" << '\n';

	while (!m_mainWindow->ShouldClose()) {
		this->update();

		m_renderer->Render();

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
