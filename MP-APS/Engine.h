#pragma once

#include "Timer.h"
#include "Core/WindowSystem.h"
#include "Core/RenderSystem.h"

#include <unordered_map>

class Engine {
public:
	// Initializes engine from an XML config file
	explicit Engine(const std::string_view configPath);

	// Disable copy + assignment
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	void AddScene(const SceneBase& scene);
	void SetActiveScene(const std::string_view sceneName);

	void Execute();

private:
	void update();
	void shutdown() const;

	Timer m_timer;

	// Core Systems
	WindowSystem m_mainWindow;
	RenderSystem m_renderer;

	// All loaded scenes stored in memory
	std::unordered_map<std::string, SceneBase> m_scenes;
	// Current scene being processed by renderer
	SceneBase* m_activeScene;
};
