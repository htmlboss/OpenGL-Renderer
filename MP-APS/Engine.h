#pragma once

#include "Timer.h"

#include "Core/WindowSystem.h"
#include "Core/RenderSystem.h"
#include "Core/GUISystem.h"

#include <unordered_map>

class Engine {
public:
	// Initializes engine from an XML config file
	explicit Engine(const std::string_view configPath);

	// Disable copy + assignment + moving
	Engine(Engine&&) = delete;
	Engine(const Engine&) = delete;
	Engine& operator=(Engine&&) = delete;
	Engine& operator=(const Engine&) = delete;

	~Engine() = default;

	void AddScene(const std::shared_ptr<SceneBase>& scene);
	void SetActiveScene(const std::string_view sceneName);

	void Execute();

private:
	void update();
	void shutdown() const;

	Timer m_timer;

	// Core Systems
	WindowSystem m_mainWindow;
	RenderSystem m_renderer;
	GUISystem m_guiSystem;

	// All loaded scenes stored in memory
	std::unordered_map<std::string, std::shared_ptr<SceneBase>> m_scenes;
	// Current scene being processed by renderer
	SceneBase* m_activeScene;
};
