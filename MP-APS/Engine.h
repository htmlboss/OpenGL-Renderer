#pragma once

#include "Timer.h"
#include "GL/GLWindow.h"
#include "GL/GLRenderer.h"

#include <memory>

class Engine {
public:
	Engine(const std::size_t width, const std::size_t height);
	~Engine() = default;

	void Execute();

private:
	void update();
	void shutdown();

	enum class engineState {
		LOADING,
		READY
	};
	engineState m_engineState;

	Timer m_timer;

	std::unique_ptr<GLWindow> m_mainWindow;
	std::unique_ptr<GLRenderer> m_renderer;
};

