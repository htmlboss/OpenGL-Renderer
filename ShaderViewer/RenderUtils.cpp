#include "RenderUtils.h"
#include <iostream>

#define GLEW_STATIC
#include <GL/glew.h>

/***********************************************************************************/
RenderUtils::RenderUtils() {
	glewExperimental = GL_TRUE;
	auto err = glewInit();
	if(err != GLEW_OK) {
		std::cerr << "GLEW failed to initialize: " << glewGetErrorString(err) << '\n';
		throw std::runtime_error("");
	}

	std::cout << "OpenGL Version: " << reinterpret_cast<const char*>(glGetString(GL_VERSION)) << std::endl;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND); // Blending
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); // Alpha blending
	glEnable(GL_FRAMEBUFFER_SRGB); // Gamma correction
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glDepthFunc(GL_LESS);

	// depth clamping for l8ter
}

/***********************************************************************************/
RenderUtils::~RenderUtils() {
}

/***********************************************************************************/
void RenderUtils::Clear() const {
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}
