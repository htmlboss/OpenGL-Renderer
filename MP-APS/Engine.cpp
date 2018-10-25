#include "Engine.h"

#include "Input.h"
#include "ViewFrustum.h"
#include "ResourceManager.h"
#include "SceneBase.h"

#include <GLFW/glfw3.h>
#include <pugixml.hpp>
#include <concrtrm.h>

#include <iostream>
#include <algorithm>
#include <execution>

/***********************************************************************************/
Engine::Engine(const std::filesystem::path& configPath) {

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
	const auto& result{ doc.load_string(ResourceManager::GetInstance().LoadTextFile(configPath).data()) };
	std::cout << "Engine config load result: " << result.description() << std::endl;

	const auto& engineNode{ doc.child("Engine") };

	std::cout << "**************************************************\n";
	std::cout << "Initializing Window...\n";
	m_window.Init(engineNode.child("Window"));

	std::cout << "**************************************************\n";
	std::cout << "Initializing OpenGL Renderer...\n";
	m_renderer.Init(engineNode.child("Renderer"));

	m_guiSystem.Init(m_window.m_window);
}

/***********************************************************************************/
void Engine::AddScene(const std::shared_ptr<SceneBase>& scene) {
	m_scenes.try_emplace(scene->GetName(), scene);
}

/***********************************************************************************/
void Engine::SetActiveScene(const std::string_view sceneName) {
	const auto& scene{ m_scenes.find(sceneName.data()) };

	if (scene == m_scenes.end()) {
		std::cerr << "Engine Error: Scene not found: " << sceneName << std::endl;
		return;
	}

	m_activeScene = scene->second.get();
	m_renderer.UpdateView(m_camera);
}

/***********************************************************************************/
void Engine::Execute() {

	if (m_activeScene == nullptr) {
		std::cerr << "Engine Error: No active scene specified!" << std::endl;
		std::abort();
	}

	std::cout << "**************************************************\n";
	std::cout << "Engine initialization complete!\n";
	std::cout << "**************************************************\n";

	// Main loop
	while (!m_window.ShouldClose()) {

		m_timer.Update(glfwGetTime());
		const auto dt{ m_timer.GetDelta() };

		Input::GetInstance().Update();

		m_window.Update();

		m_camera.Update(dt);

		m_activeScene->Update(dt);

		m_renderer.Update(m_camera);

		const auto& renderList{ cullViewFrustum() };
		m_renderer.Render(m_camera, renderList.cbegin(), renderList.cend(), *m_activeScene, false);

		m_guiSystem.Render();

		m_window.SwapBuffers();
	}

	shutdown();
}

/***********************************************************************************/
void Engine::shutdown() const {
	m_guiSystem.Shutdown();
	m_renderer.Shutdown();
	ResourceManager::GetInstance().ReleaseAllResources();
	m_window.Shutdown();
}

/***********************************************************************************/
std::vector<ModelPtr> Engine::cullViewFrustum() const {
	std::vector<ModelPtr> renderList;

	const auto& dims{ m_window.GetFramebufferDims() };
	const ViewFrustum viewFrustum(m_camera.GetViewMatrix(), m_camera.GetProjMatrix(dims.first, dims.second));

	std::for_each(std::execution::parallel_unsequenced_policy(), m_activeScene->m_sceneModels.cbegin(), m_activeScene->m_sceneModels.cend(), 
		[&](const auto& model) {
		const auto result{ viewFrustum.TestIntersection(model->GetBoundingBox()) };

		if (result == BoundingVolume::TestResult::INSIDE || result == BoundingVolume::TestResult::INTERSECT) {
			renderList.push_back(model);
		}
	});

	return renderList;
}
