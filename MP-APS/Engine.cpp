#include "Engine.h"
#include "Input.h"

#include <iostream>

#include <concrtrm.h>

/***********************************************************************************/
Engine::Engine(const size_t width, const size_t height, const bool fullscreen) : m_engineState(engineState::LOADING),
																				 m_mainWindow(width, height, "MP-APS", fullscreen),
																				 m_renderer(width, height),
																				 m_scene(width, height) {
	std::cout << "Available processor cores: " << Concurrency::GetProcessorCount() << '\n';
	m_renderer.InitView(m_scene.GetCamera());
	m_scene.Init();
}

/***********************************************************************************/
void Engine::Execute() {

	m_engineState = engineState::READY;
	std::cout << "\nEngine initialization complete!\n";
	std::cout << "**************************************************\n\n";

	while (!m_mainWindow.ShouldClose()) {
		update();

		m_renderer.Render(m_scene.GetCamera(), m_scene.GetRenderData());

		m_mainWindow.SwapBuffers();
	}

	shutdown();
}

/***********************************************************************************/
void Engine::update() {
	Input::GetInstance().Update();
	m_mainWindow.Update();
	m_timer.Update(glfwGetTime());

	m_scene.Update(m_timer.GetDelta());
	m_renderer.Update(m_scene.GetCamera(), m_timer.GetDelta());
}

/***********************************************************************************/
void Engine::shutdown() {
	m_renderer.Shutdown();
	m_mainWindow.DestroyWindow();
}
