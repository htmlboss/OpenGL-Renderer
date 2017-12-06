#pragma once

#include "Timer.h"
#include "Core/WindowSystem.h"
#include "Core/RenderSystem.h"

#include "SceneBase.h"

#include <memory>

class Engine {
public:
	// Initializes engine from an XML config file
	explicit Engine(const std::string_view configPath);

	// Disable copy + assignment
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	void Execute();

private:

	void update();
	void shutdown() const;

	Timer m_timer;

	WindowSystem m_mainWindow;
	RenderSystem m_renderer;
	std::unique_ptr<SceneBase> m_scene;
};
