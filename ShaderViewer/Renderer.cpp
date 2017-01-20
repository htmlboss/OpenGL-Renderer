#include "Renderer.h"

#include <iostream>
#define GLEW_STATIC
#include <GL/glew.h>

/***********************************************************************************/
Renderer::Renderer(const size_t width, const size_t height) : IRenderer(width, height) {
	
	glewExperimental = GL_TRUE;
	auto err = glewInit();
	if (err != GLEW_OK) {
		std::cerr << "GLEW failed to initialize: " << glewGetErrorString(err) << '\n';
		throw std::runtime_error("");
	}

	std::cout << "OpenGL Version: " << reinterpret_cast<const char*>(glGetString(GL_VERSION)) << std::endl;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND); // Blending
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Alpha blending
	glEnable(GL_FRAMEBUFFER_SRGB); // Gamma correction
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LESS);

	// depth clamping for l8ter
	
	glViewport(0, 0, width, height);

	m_gBuffer = std::make_unique<GBuffer>(GBuffer(width, height));
}

/***********************************************************************************/
Renderer::~Renderer() {
}

/***********************************************************************************/
void Renderer::ClearColor(const float r, const float g, const float b, const float a) const {
	glClearColor(r, g, b, a);
}

/***********************************************************************************/
void Renderer::Resize(const size_t width, const size_t height) {
	m_width = width;
	m_height = height;
	glViewport(0, 0, width, height);
}
