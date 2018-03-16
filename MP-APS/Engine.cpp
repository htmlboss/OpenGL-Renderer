#include "Engine.h"

#include "Input.h"
#include "ResourceManager.h"
#include "SceneBase.h"

#include <GLFW/glfw3.h>
#include <pugixml.hpp>
#include <concrtrm.h>

#include <iostream>

/***********************************************************************************/
Engine::Engine(const std::string_view configPath) : m_mainWindow{},
													m_renderer{},
													m_activeScene{nullptr} {

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

	m_guiSystem.Init(m_mainWindow.m_window);
}

/***********************************************************************************/
void Engine::AddScene(const std::shared_ptr<SceneBase>& scene) {
	m_scenes.try_emplace(scene->GetName(), scene);
}

/***********************************************************************************/
void Engine::SetActiveScene(const std::string_view sceneName) {
	const auto& scene = m_scenes.find(sceneName.data());

	if (scene == m_scenes.end()) {
		std::cerr << "Engine Error: Scene not found: " << sceneName << std::endl;
		return;
	}

	m_activeScene = scene->second.get();
	m_renderer.InitScene(*m_activeScene);
}

/***********************************************************************************/
void Engine::Execute() {

	if (m_activeScene == nullptr) {
		std::cerr << "Engine Error: No active scene specified!" << std::endl;
		std::abort();
	}

	std::cout << "\n**************************************************\n";
	std::cout << "Engine initialization complete!\n";
	std::cout << "**************************************************\n" << std::endl;

	// Main loop
	while (!m_mainWindow.ShouldClose()) {
		update();

		m_renderer.Render(*m_activeScene, false);

		m_guiSystem.Render();

		m_mainWindow.SwapBuffers();
	}

	shutdown();
}

/***********************************************************************************/
void Engine::update() {
	m_timer.Update(glfwGetTime());
	Input::GetInstance().Update();
	m_mainWindow.Update();

	m_activeScene->Update(m_timer.GetDelta());
	m_renderer.Update(*m_activeScene, m_timer.GetDelta());
}

/***********************************************************************************/
void Engine::shutdown() const {
	m_guiSystem.Shutdown();
	m_renderer.Shutdown();
	ResourceManager::GetInstance().ReleaseAllResources();
	m_mainWindow.Shutdown();
}
