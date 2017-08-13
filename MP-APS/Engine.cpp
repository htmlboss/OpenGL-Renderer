#include "Engine.h"
#include "Input.h"
#include "ResourceManager.h"

#include <pugixml.hpp>
#include <concrtrm.h>

#include <iostream>

/***********************************************************************************/
Engine::Engine(const std::string_view configPath) : m_engineState(engineState::LOADING),
													m_mainWindow() {

	std::cout << "Engine starting up...\n";
	std::cout << "Loading Engine config file...\n";
	std::cout << "Available processor cores: " << Concurrency::GetProcessorCount() << '\n';

	pugi::xml_document doc;
	const auto result = doc.load_string(ResourceManager::GetInstance().LoadTextFile(configPath).data());
	std::cout << "Engine config load result: " << result.description() << std::endl;

	// Global Engine parameters.
	const auto engine = doc.child("Engine");
	m_width = engine.attribute("width").as_ullong();
	m_height = engine.attribute("height").as_ullong();

	// Window parameters
	m_mainWindow.Init(m_width, m_height, engine.child("Window"));

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
	std::cout << "**************************************************\n" << std::endl;

	// Main loop
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
