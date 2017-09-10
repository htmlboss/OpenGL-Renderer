#include "Engine.h"
#include "Input.h"
#include "ResourceManager.h"

#include <GLFW/glfw3.h>
#include <pugixml.hpp>
#include <concrtrm.h>

#include <iostream>

/***********************************************************************************/
Engine::Engine(const std::string_view configPath) : m_engineState(engineState::LOADING),
													m_mainWindow{},
													m_renderer{} {

	std::cout << "**************************************************\n";
	std::cout << "Engine starting up...\n";
	std::cout << "**************************************************\n";
	std::cout << "Available processor cores: " << Concurrency::GetProcessorCount() << '\n';
	std::cout << "**************************************************\n";
	std::cout << "Loading Engine config file...\n";
	
	pugi::xml_document doc;
	const auto result = doc.load_string(ResourceManager::GetInstance().LoadTextFile(configPath).data());
	std::cout << "Engine config load result: " << result.description() << std::endl;

	const auto engineNode = doc.child("Engine");

	std::cout << "**************************************************\n";
	std::cout << "Initializing Window...\n";
	m_mainWindow.Init(engineNode.child("Window"));

	std::cout << "**************************************************\n";
	std::cout << "Initializing OpenGL Renderer...\n";
	m_renderer.Init(engineNode.child("Renderer"));
	
	std::cout << "**************************************************\n";
	std::cout << "Initializing scene...\n";
	m_scene = std::make_unique<Scene>(1280, 720);

	m_renderer.InitView(m_scene->GetCamera());
	m_scene->Init();
}

/***********************************************************************************/
void Engine::Execute() {

	m_engineState = engineState::READY;
	std::cout << "\n**************************************************\n";
	std::cout << "Engine initialization complete!\n";
	std::cout << "**************************************************\n" << std::endl;

	// Main loop
	while (!m_mainWindow.ShouldClose()) {
		Update();

		m_renderer.Render(m_scene->GetCamera(), m_scene->GetRenderData());

		m_mainWindow.SwapBuffers();
	}

	shutdown();
}

/***********************************************************************************/
void Engine::Update() {
	Input::GetInstance().Update();
	m_mainWindow.Update(0.0);
	m_timer.Update(glfwGetTime());

	m_scene->Update(m_timer.GetDelta());
	m_renderer.Update(m_scene->GetCamera(), m_timer.GetDelta());
}

/***********************************************************************************/
void Engine::shutdown() {
	m_renderer.Shutdown();
	m_mainWindow.DestroyWindow();
}
