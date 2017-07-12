#pragma once

#include "Timer.h"
#include "GL/GLWindow.h"
#include "GL/GLRenderer.h"

#include "Scene.h"


class Engine {
public:
	Engine(const size_t width, const size_t height, const bool fullscreen = false);
	~Engine() = default;

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

	Timer m_timer;

	GLWindow m_mainWindow;
	GLRenderer m_renderer;
	Scene m_scene;
};
