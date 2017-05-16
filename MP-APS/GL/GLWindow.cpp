#include "GLWindow.h"

#include "../Input.h"

#include <GLFW/glfw3.h>
#include <iostream>

/***********************************************************************************/
GLWindow::GLWindow(const std::size_t width, const std::size_t height, const std::string_view windowName, const bool fullscreen) :	IWindow(width, height),
																																	m_window(nullptr) {
	std::cout << "**************************************************" << '\n';
#define genericInputCallback(functionName)\
	[](GLFWwindow* window, auto... args) {\
		const auto ptr = static_cast<Input*>(glfwGetWindowUserPointer(window));\
		if (ptr->functionName) { ptr->functionName(args...); }\
	}

	if (!glfwInit()) {
		throw std::runtime_error("Failed to start GLFW.");
	}

	std::cout << "GLFW Version: " << glfwGetVersionString() << '\n';
	
	glfwSetErrorCallback([](int errorCode, const char* message) { std::cerr << message; throw std::runtime_error(message); });
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	
	if (fullscreen) {
			m_window = glfwCreateWindow(width, height, windowName.data(), glfwGetPrimaryMonitor(), nullptr);
	} 
	else {
		m_window = glfwCreateWindow(width, height, windowName.data(), nullptr, nullptr);	
	}
	if (!m_window) {
		throw std::runtime_error("Failed to create GLFW window.");
	}
	glfwMakeContextCurrent(m_window);
	glfwFocusWindow(m_window);
	glfwSetWindowSizeCallback(m_window, genericInputCallback(Input::GetInstance()->windowResized));
	glfwSetKeyCallback(m_window, genericInputCallback(Input::GetInstance()->keyPressed));
	glfwSetCursorPosCallback(m_window, genericInputCallback(Input::GetInstance()->mouseMoved));

	// Center window
	const auto mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetWindowPos(m_window, (mode->width / 2) - width / 2, (mode->height / 2) - height / 2);

	this->DisableCursor();
}

/***********************************************************************************/
void GLWindow::PollEvents() const {
	glfwPollEvents();
}

/***********************************************************************************/
void GLWindow::SetWindowPos(const std::size_t x, const std::size_t y) const {
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
void GLWindow::EnableVSync() {
	glfwSwapInterval(1);
}

/***********************************************************************************/
void GLWindow::DisableVSync() {
	glfwSwapInterval(0);
}

/***********************************************************************************/
void GLWindow::Update() const {
	PollEvents();

	if (Input::GetInstance()->IsKeyPressed(GLFW_KEY_ESCAPE)) {
		m_shouldWindowClose = true;
		glfwSetWindowShouldClose(m_window, true);
	}
}