#pragma once

#include "Timer.h"
#include "GL/GLWindow.h"
#include "GL/GLRenderer.h"
#include "Scene.h"

#include <memory>

class Engine {
public:
	Engine(const std::size_t width, const std::size_t height, const bool fullscreen = false);
	~Engine() = default;

	void Execute();

private:
	void update();
	void shutdown() const;

	enum class engineState {
		LOADING,
		READY,
		PAUSED
	};
	engineState m_engineState;

	Timer m_timer;

	std::unique_ptr<GLWindow> m_mainWindow;
	std::unique_ptr<GLRenderer> m_renderer;
	std::unique_ptr<Scene> m_scene;
};

