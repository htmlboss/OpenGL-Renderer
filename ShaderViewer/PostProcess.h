#pragma once

#include <gl/glew.h>

#include "Shader.h"
#include "FrameBuffer.h"

class PostProcess {
public:
	PostProcess(const std::string& screenVertShader, const std::string& screenPixelShader);
	~PostProcess();

	// Render processed image to a screen quad
	void RendertoScreen(const FrameBuffer& fb) const;

private:
	Shader m_screenShader;
	GLuint m_screenVAO, m_screenVBO;

	const GLfloat m_quadVertices[20] = {
		// Positions        // Texture Coords
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
	};
};

