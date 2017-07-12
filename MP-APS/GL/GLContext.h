#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Creates OpenGL Context.
class GLContext {
public:
	GLContext() {
		if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
			throw std::runtime_error("Failed to start GLAD.\n");
		}
	}

	static auto GetGLVersion() { return glGetString(GL_VERSION); }
	static auto GetGLSLVersion() { return glGetString(GL_SHADING_LANGUAGE_VERSION); }
	static auto GetGLVendor() { return glGetString(GL_VENDOR); }
	static auto GetGLRenderer() { return glGetString(GL_RENDERER); }
};

