#include "Engine.h"

#include "Input.h"
#include "ResourceManager.h"

#include <GLFW/glfw3.h>
#include <pugixml.hpp>
#include <concrtrm.h>

#include <iostream>

/***********************************************************************************/
Engine::Engine(const std::string_view configPath) : m_mainWindow{},
													m_renderer{} {

	std::cout << "**************************************************\n";
	std::cout << "Engine starting up...\n";
#ifdef _DEBUG
	std::cout << "DEBUG MODE!!\n";
#endif
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
	m_scene = std::make_unique<SceneBase>(1280, 720);

	m_renderer.InitView(m_scene->GetCamera());
	m_scene->Init();
}

/***********************************************************************************/
void Engine::Execute() {

	std::cout << "\n**************************************************\n";
	std::cout << "Engine initialization complete!\n";
	std::cout << "**************************************************\n" << std::endl;

	// Main loop
	while (!m_mainWindow.ShouldClose()) {
		update();

		m_renderer.Render(*m_scene, false);

		m_mainWindow.SwapBuffers();
	}

	shutdown();
}

/***********************************************************************************/
void Engine::update() {
	m_timer.Update(glfwGetTime());
	Input::GetInstance().Update();
	m_mainWindow.Update();

	m_scene->Update(m_timer.GetDelta());
	m_renderer.Update(m_scene->GetCamera(), m_timer.GetDelta());
}

/***********************************************************************************/
void Engine::shutdown() const {
	m_renderer.Shutdown();
	ResourceManager::GetInstance().ReleaseAllResources();
	m_mainWindow.Shutdown();
}
