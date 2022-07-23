#include "Engine.h"

#include "Timer.h"
#include "Input.h"
#include "ViewFrustum.h"
#include "ResourceManager.h"
#include "SceneBase.h"
#include "FrameStats.h"
#include "Platform/Platform.h"

#include <GLFW/glfw3.h>
#include <pugixml.hpp>

#include <iostream>
#include <algorithm>
#include <execution>
#include <thread>

/***********************************************************************************/
void connectWindowInstanceToInput(GLFWwindow* window) {
	const auto resizeCallback = [](GLFWwindow* w, auto width, auto height) {
		Input::GetInstance().windowResized(width, height);
	};
	glfwSetWindowSizeCallback(window, resizeCallback);
	
	const auto keyCallback = [](GLFWwindow* w, auto key, auto scancode, auto action, auto mode) {
		Input::GetInstance().keyPressed(key, scancode, action, mode);
	};
	glfwSetKeyCallback(window, keyCallback);

	const auto cursorPosCallback = [](GLFWwindow* w, auto xPos, auto yPos) {
		Input::GetInstance().mouseMoved(xPos, yPos);
	};
	glfwSetCursorPosCallback(window, cursorPosCallback);
}

/***********************************************************************************/
double frameTimeMilliseconds(const unsigned int numFramesRendered) {
	return 1000.0 / static_cast<double>(numFramesRendered);
}

/***********************************************************************************/
double framesPerSecond(const double frameTimeMilliseconds) {
	return 1.0 / (frameTimeMilliseconds / 1000.0);
}

/***********************************************************************************/
Engine::Engine(const std::filesystem::path& configPath) {

	std::cout << "**************************************************\n";
	std::cout << "Engine starting up...\n";
#ifdef _DEBUG
	std::cout << "DEBUG MODE!!\n";
#endif
	std::cout << "**************************************************\n";
	std::cout << "Available processor cores: " << std::thread::hardware_concurrency() << '\n';
	std::cout << "**************************************************\n";
	std::cout << "Loading Engine config file...\n";
	
	pugi::xml_document doc;
	const auto& result{ doc.load_string(ResourceManager::GetInstance().LoadTextFile(configPath).data()) };
	std::cout << "Engine config load result: " << result.description() << std::endl;

	const auto& engineNode{ doc.child("Engine") };

	std::cout << "**************************************************\n";
	std::cout << "Initializing Window...\n";
	auto* window{ m_window.Init(engineNode.child("Window")) };
	connectWindowInstanceToInput(window);

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

	bool hasOneSecondPassed{ false };
	Timer timer(1.0, [&]() {
		hasOneSecondPassed = true;
	});

	// Main loop
	unsigned int numFramesRendered{ 0 };
	FrameStats frameStats;
	bool showFrameStats{ true };
	while (!m_window.ShouldClose()) {
		timer.Update(glfwGetTime());		
		
		if (hasOneSecondPassed) {
			const auto frameTime{ frameTimeMilliseconds(numFramesRendered) };		
			frameStats.frameTimeMilliseconds = frameTime;
			frameStats.videoMemoryUsageKB = m_renderer.GetVideoMemUsageKB();
			frameStats.ramUsageKB = Platform::maxRSSKb();

			numFramesRendered = 0;
			hasOneSecondPassed = false;
		}

		const auto dt{ timer.GetDelta() };

		Input::GetInstance().Update();

		m_window.Update();
		
		const auto& [width, height] = m_window.GetFramebufferDims();

		m_camera.Update(dt);

		m_activeScene->Update(dt);

		m_renderer.Update(m_camera);

		const auto& renderList{ cullViewFrustum() };
		m_renderer.Render(m_camera, renderList.cbegin(), renderList.cend(), *m_activeScene, false);

		m_guiSystem.Render(width, height, frameStats);

		m_window.SwapBuffers();

		++numFramesRendered;
	}

	shutdown();
}

/***********************************************************************************/
void Engine::shutdown() {
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

	std::for_each(std::execution::par_unseq, m_activeScene->m_sceneModels.cbegin(), m_activeScene->m_sceneModels.cend(), 
		[&](const auto& model) {
		const auto result{ viewFrustum.TestIntersection(model->GetBoundingBox()) };

		if (result == BoundingVolume::TestResult::INSIDE || result == BoundingVolume::TestResult::INTERSECT) {
			renderList.push_back(model);
		}
	});

	return renderList;
}
