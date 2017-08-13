#pragma once

#include "Timer.h"
#include "GL/GLWindow.h"
#include "GL/GLRenderer.h"

#include "Scene.h"

#include <memory>

class Engine {
public:
	// Initializes engine from an XML config file
	explicit Engine(const std::string_view configPath);

	// Disable copy + assignment
	Engine(const Engine&) = delete;
	Engine& operator=(const Engine&) = delete;

	~Engine() = default;

	// Runs game loop
	void Execute();

private:
	void update();
	void shutdown();

	enum class engineState {
		LOADING,
		READY,
		PAUSED
	};

	engineState m_engineState;

	std::size_t m_width, m_height;

	Timer m_timer;

	GLWindow m_mainWindow;
	std::unique_ptr<GLRenderer> m_renderer;
	std::unique_ptr<Scene> m_scene;
};
