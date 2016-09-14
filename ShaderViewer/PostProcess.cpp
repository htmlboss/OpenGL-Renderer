#include "PostProcess.h"


/***********************************************************************************/
PostProcess::PostProcess(const std::string& screenVertShader, const std::string& screenPixelShader) : 
	m_screenShader("", screenVertShader, screenPixelShader) {

	glGenVertexArrays(1, &m_screenVAO);
	glGenBuffers(1, &m_screenVBO);
	glBindVertexArray(m_screenVAO);
	glBindBuffer(GL_ARRAY_BUFFER, m_screenVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_quadVertices), &m_quadVertices, GL_STATIC_DRAW); // sizeof is size in BYTES. .size() is length of container >.>
	// Positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	// Tex-Coords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
	
	glBindVertexArray(0);
}

/***********************************************************************************/
PostProcess::~PostProcess() {
}

/***********************************************************************************/
void PostProcess::RendertoScreen(const FrameBuffer& fb) const {
	m_screenShader.Use();
	glBindVertexArray(m_screenVAO);
	glBindTexture(GL_TEXTURE_2D, fb.GetID());	// Use framebuffer ID as texture
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}
