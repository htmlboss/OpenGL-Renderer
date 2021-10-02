#pragma once
#include "Camera.h"

#include "Core/WindowSystem.h"
#include "Core/RenderSystem.h"
#include "Core/GUISystem.h"

#include <unordered_map>
#include <filesystem>

class Engine {
public:
	// Initializes engine from an XML config file
	explicit Engine(const std::filesystem::path& configPath);

	void AddScene(const std::shared_ptr<SceneBase>& scene);
	void SetActiveScene(const std::string_view sceneName);

	// Load scene and run update loop
	void Execute();

private:
	void shutdown() const;

	// Performs view-frustum culling.
	// Returns meshes visible by the camera.
	std::vector<ModelPtr> cullViewFrustum() const;

	Camera m_camera;

	// Core Systems
	WindowSystem m_window;
	RenderSystem m_renderer;
	GUISystem m_guiSystem;

	// All loaded scenes stored in memory
	std::unordered_map<std::string, std::shared_ptr<SceneBase>> m_scenes;
	// Current scene being processed by renderer
	SceneBase* m_activeScene{ nullptr };
};
