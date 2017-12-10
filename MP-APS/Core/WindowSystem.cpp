#include "WindowSystem.h"
#include "../Input.h"

#include <GLFW/glfw3.h>
#include <pugixml.hpp>

#include <iostream>

/***********************************************************************************/
void WindowSystem::Init(const pugi::xml_node& windowNode) {

	// Gross lambda to connect the Input singleton to GLFW callbacks
#define genericInputCallback(functionName)\
	[](GLFWwindow* window, const auto... args) {\
		const auto ptr = static_cast<Input*>(glfwGetWindowUserPointer(window));\
		if (ptr->functionName) { ptr->functionName(args...); }\
	}

	if (!glfwInit()) {
		std::cerr <<  "Failed to start GLFW.";
		std::abort();
	}

#ifdef _DEBUG
	std::cout << "GLFW Version: " << glfwGetVersionString() << '\n';
#endif

	glfwSetErrorCallback([](const auto errorCode, const auto* message) {std::cerr << "GLFW Error: " << errorCode << ". " << message << std::endl; });
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, windowNode.attribute("major").as_int());
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, windowNode.attribute("minor").as_int());
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	const auto width = windowNode.attribute("width").as_uint();
	const auto height = windowNode.attribute("height").as_uint();

	// Determine if fullscreen is requested
	if (windowNode.attribute("fullscreen").as_bool()) {
		m_window = glfwCreateWindow(width, height, windowNode.attribute("title").as_string(), glfwGetPrimaryMonitor(), nullptr);
	}
	else {
		m_window = glfwCreateWindow(width, height, windowNode.attribute("title").as_string(), nullptr, nullptr);
	}

	if (!m_window) {
		std::cerr << "Failed to create GLFW window.";
		std::abort();
	}

	glfwMakeContextCurrent(m_window);
	glfwFocusWindow(m_window);
	glfwSetWindowSizeCallback(m_window, genericInputCallback(Input::GetInstance().windowResized));
	glfwSetKeyCallback(m_window, genericInputCallback(Input::GetInstance().keyPressed));
	glfwSetCursorPosCallback(m_window, genericInputCallback(Input::GetInstance().mouseMoved));

	// Center window
	const auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(m_window, (mode->width / 2) - width / 2, (mode->height / 2) - height / 2);

	DisableCursor();
}

/***********************************************************************************/
void WindowSystem::SetWindowPos(const std::size_t x, const std::size_t y) const {
	glfwSetWindowPos(m_window, x, y);
}

/***********************************************************************************/
void WindowSystem::SwapBuffers() const {
	glfwSwapBuffers(m_window);
}

/***********************************************************************************/
void WindowSystem::EnableCursor() const {
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

/***********************************************************************************/
void WindowSystem::DisableCursor() const {
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

/***********************************************************************************/
void WindowSystem::SetVsync(const bool vsync) const {
	glfwSwapInterval(static_cast<int>(vsync));
}

/***********************************************************************************/
void WindowSystem::Update() {
	glfwPollEvents();

	// Check if the window needs to be closed
	if (Input::GetInstance().IsKeyPressed(GLFW_KEY_ESCAPE) || glfwWindowShouldClose(m_window)) {
		m_shouldWindowClose = true;
		glfwSetWindowShouldClose(m_window, true);
	}
}

/***********************************************************************************/
void WindowSystem::Shutdown() const {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}
