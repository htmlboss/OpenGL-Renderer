#include "GLWindow.h"
#include "../Input.h"

#include <GLFW/glfw3.h>
#include <pugixml.hpp>

#include <iostream>

/***********************************************************************************/
void GLWindow::Init(const pugi::xml_node& windowNode) {

#define genericInputCallback(functionName)\
	[](GLFWwindow* window, const auto... args) {\
		const auto ptr = static_cast<Input*>(glfwGetWindowUserPointer(window));\
		if (ptr->functionName) { ptr->functionName(args...); }\
	}

	if (!glfwInit()) {
		throw std::runtime_error("Failed to start GLFW.");
	}

#ifdef _DEBUG
	std::cout << "GLFW Version: " << glfwGetVersionString() << '\n';
#endif

	glfwSetErrorCallback([](const auto errorCode, const auto* message) {std::cerr << "GLFW Error: " << errorCode << ". " << message << std::endl; });
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, windowNode.attribute("major").as_int());
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, windowNode.attribute("minor").as_int());
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	const auto width = windowNode.attribute("width").as_uint();
	const auto height = windowNode.attribute("height").as_uint();

	if (windowNode.attribute("fullscreen").as_bool()) {
		m_window = glfwCreateWindow(width, height, windowNode.attribute("title").as_string(), glfwGetPrimaryMonitor(), nullptr);
	}
	else {
		m_window = glfwCreateWindow(width, height, windowNode.attribute("title").as_string(), nullptr, nullptr);
	}

	if (!m_window) {
		throw std::runtime_error("Failed to create GLFW window.");
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
void GLWindow::SetWindowPos(const size_t x, const size_t y) const {
	glfwSetWindowPos(m_window, x, y);
}

/***********************************************************************************/
void GLWindow::SwapBuffers() const {
	glfwSwapBuffers(m_window);
}

/***********************************************************************************/
void GLWindow::DestroyWindow() const {
	glfwDestroyWindow(m_window);
	glfwTerminate();
}

/***********************************************************************************/
void GLWindow::EnableCursor() const {
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

/***********************************************************************************/
void GLWindow::DisableCursor() const {
	glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

/***********************************************************************************/
void GLWindow::EnableVSync() const {
	glfwSwapInterval(1);
}

/***********************************************************************************/
void GLWindow::DisableVSync() const {
	glfwSwapInterval(0);
}

/***********************************************************************************/
void GLWindow::Update(const double delta) {
	glfwPollEvents();

	if (Input::GetInstance().IsKeyPressed(GLFW_KEY_ESCAPE) || glfwWindowShouldClose(m_window)) {
		m_shouldWindowClose = true;
		glfwSetWindowShouldClose(m_window, true);
	}
}
