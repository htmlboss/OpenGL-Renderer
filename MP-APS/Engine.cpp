#include "Engine.h"
#include "Input.h"

#include <pugixml.hpp>
#include <concrtrm.h>

#include <iostream>

/***********************************************************************************/
Engine::Engine(const std::string_view configPath) : m_engineState(engineState::LOADING),
													m_mainWindow() {

	std::cout << "Available processor cores: " << Concurrency::GetProcessorCount() << '\n';
	
	std::cout << "Loading Engine config file...\n";

	pugi::xml_document doc;
	const auto result = doc.load_file(configPath.data());

	std::cout << "Engine config load result: " << result.description() << std::endl;

	// Global Engine parameters.
	const auto engine = doc.child("Engine");
	m_width = engine.attribute("width").as_ullong();
	m_height = engine.attribute("height").as_ullong();

	// Window parameters
	const auto window = doc.child("Engine").child("Window");
	m_mainWindow.Init(	m_width,
						m_height,
						window.attribute("title").as_string(),
						window.attribute("fullscreen").as_bool()
					);

	// Renderer parameters
	m_renderer = std::make_unique<GLRenderer>(m_width, m_height);
	
	// Scene parameters
	m_scene = std::make_unique<Scene>(m_width, m_height);

	m_renderer->InitView(m_scene->GetCamera());
	m_scene->Init();
}

/***********************************************************************************/
void Engine::Execute() {

	m_engineState = engineState::READY;
	std::cout << "\nEngine initialization complete!\n";
	std::cout << "**************************************************\n\n";

	while (!m_mainWindow.ShouldClose()) {
		update();

		m_renderer->Render(m_scene->GetCamera(), m_scene->GetRenderData());

		m_mainWindow.SwapBuffers();
	}

	shutdown();
}

/***********************************************************************************/
void Engine::update() {
	Input::GetInstance().Update();
	m_mainWindow.Update();
	m_timer.Update(glfwGetTime());

	m_scene->Update(m_timer.GetDelta());
	m_renderer->Update(m_scene->GetCamera(), m_timer.GetDelta());
}

/***********************************************************************************/
void Engine::shutdown() {
	m_renderer->Shutdown();
	m_mainWindow.DestroyWindow();
}
