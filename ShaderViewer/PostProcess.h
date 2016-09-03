#pragma once

#include <array>
#include <string>
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

	// Vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates
	const std::array<GLfloat, 24> m_quadVertices = {
		// Positions   TexCoords

		// Triangle 1
		-1.0f,  1.0f,  0.0f, 1.0f, // Top-left
		-1.0f, -1.0f,  0.0f, 0.0f, // Bottom-left
		1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right

		// Triangle 2
		-1.0f,  1.0f,  0.0f, 1.0f, // Top-left
		1.0f, -1.0f,  1.0f, 0.0f, // Bottom-right
		1.0f,  1.0f,  1.0f, 1.0f // Top-right
	};
};

